#!/usr/bin/env node

import { readdirSync, readFileSync, statSync } from 'node:fs'
import { join, relative } from 'node:path'

const BASE_LOCALE = 'en'
const TARGET_LOCALES = ['ja', 'ru', 'fr']
const CONTENT_DIR = join(process.cwd(), 'content')
const ALLOWED_FILE_NAMES = new Set(['.navigation.yml'])
const ALLOWED_FILE_EXTENSIONS = ['.md']

function walkFiles(dir) {
  const results = []
  for (const entry of readdirSync(dir)) {
    const fullPath = join(dir, entry)
    const stats = statSync(fullPath)
    if (stats.isDirectory()) {
      results.push(...walkFiles(fullPath))
      continue
    }
    const isAllowedName = ALLOWED_FILE_NAMES.has(entry)
    const isAllowedExt = ALLOWED_FILE_EXTENSIONS.some((ext) => entry.endsWith(ext))
    if (isAllowedName || isAllowedExt) {
      results.push(fullPath)
    }
  }
  return results
}

function loadLocaleFiles(locale) {
  const localeRoot = join(CONTENT_DIR, locale)
  const files = walkFiles(localeRoot)
  const relativeFiles = files.map((abs) => relative(localeRoot, abs).replaceAll('\\\\', '/'))
  return new Set(relativeFiles)
}

function normalizeMarkdownText(source) {
  let text = source
  text = text.replace(/^---\n[\s\S]*?\n---\n?/u, '')
  text = text.replace(/```[\s\S]*?```/gu, ' ')
  text = text.replace(/`[^`]*`/gu, ' ')
  text = text.replace(/!\[[^\]]*\]\([^)]*\)/gu, ' ')
  text = text.replace(/\[([^\]]+)\]\([^)]*\)/gu, '$1')
  text = text.replace(/[>#*_~\-]/gu, ' ')
  text = text.replace(/\s+/gu, ' ').trim()
  return text
}

function tokenizeLatinWords(text) {
  const matches = text.match(/[A-Za-z][A-Za-z'-]{1,}/g)
  return matches ? matches.map((word) => word.toLowerCase()) : []
}

function tokenizeWordsLoose(text) {
  const matches = text.toLowerCase().match(/[\p{L}][\p{L}\p{N}'-]{1,}/gu)
  return matches ?? []
}

function countByRegex(text, regex) {
  const matches = text.match(regex)
  return matches ? matches.length : 0
}

function readMarkdown(locale, relPath) {
  return readFileSync(join(CONTENT_DIR, locale, relPath), 'utf8')
}

function jaccardSimilarity(aWords, bWords) {
  const aSet = new Set(aWords)
  const bSet = new Set(bWords)
  if (aSet.size === 0 || bSet.size === 0) {
    return 0
  }

  let intersection = 0
  for (const token of aSet) {
    if (bSet.has(token)) {
      intersection += 1
    }
  }

  const union = aSet.size + bSet.size - intersection
  return union === 0 ? 0 : intersection / union
}

function overlapCoefficient(aWords, bWords) {
  const aSet = new Set(aWords)
  const bSet = new Set(bWords)
  if (aSet.size === 0 || bSet.size === 0) {
    return 0
  }

  let intersection = 0
  for (const token of aSet) {
    if (bSet.has(token)) {
      intersection += 1
    }
  }

  return intersection / Math.min(aSet.size, bSet.size)
}

const englishStopwords = new Set([
  'the', 'and', 'for', 'with', 'this', 'that', 'from', 'your', 'are', 'you',
  'into', 'when', 'how', 'what', 'where', 'which', 'using', 'use', 'used',
  'tool', 'tools', 'page', 'docs', 'documentation', 'test', 'tests', 'project',
])

const frenchStopwords = new Set([
  'le', 'la', 'les', 'de', 'des', 'du', 'et', 'ou', 'pour', 'avec', 'dans',
  'sur', 'est', 'sont', 'une', 'un', 'ce', 'cette', 'ces', 'vous', 'nous',
  'documentation', 'intégration',
])

function stopwordRatio(words, dictionary) {
  if (words.length === 0) {
    return 0
  }
  let hits = 0
  for (const word of words) {
    if (dictionary.has(word)) {
      hits += 1
    }
  }
  return hits / words.length
}

function evaluateTranslationSignals(locale, relPath) {
  if (!relPath.endsWith('.md')) {
    return null
  }

  const enTextRaw = readMarkdown(BASE_LOCALE, relPath)
  const localeTextRaw = readMarkdown(locale, relPath)
  const enText = normalizeMarkdownText(enTextRaw)
  const localeText = normalizeMarkdownText(localeTextRaw)

  const enLatinWords = tokenizeLatinWords(enText)
  const localeLatinWords = tokenizeLatinWords(localeText)

  const enWordsLoose = tokenizeWordsLoose(enText)
  const localeWordsLoose = tokenizeWordsLoose(localeText)

  const exactCopy = enText.toLowerCase() === localeText.toLowerCase() && enText.length > 120
  const jaccard = jaccardSimilarity(enLatinWords, localeLatinWords)
  const overlap = overlapCoefficient(enLatinWords, localeLatinWords)

  const englishRatioInLocale = stopwordRatio(localeLatinWords, englishStopwords)
  const frenchRatioInLocale = stopwordRatio(localeLatinWords, frenchStopwords)

  const localeScriptCount = locale === 'ja'
    ? countByRegex(localeText, /[\p{Script=Hiragana}\p{Script=Katakana}\p{Script=Han}]/gu)
    : locale === 'ru'
    ? countByRegex(localeText, /\p{Script=Cyrillic}/gu)
    : 0

  const likelyUntranslated = exactCopy
    || (locale !== 'fr' && localeScriptCount < 35 && localeLatinWords.length > 90 && overlap > 0.72)
    || (locale === 'fr' && localeLatinWords.length > 120 && overlap > 0.78 && englishRatioInLocale >= frenchRatioInLocale)
    || (localeLatinWords.length > 140 && overlap > 0.86 && jaccard > 0.68)

  if (!likelyUntranslated) {
    return null
  }

  return {
    path: relPath,
    exactCopy,
    overlap: Number(overlap.toFixed(3)),
    jaccard: Number(jaccard.toFixed(3)),
    localeScriptCount,
    latinWordCount: localeLatinWords.length,
  }
}

function runAudit() {
  const enFiles = loadLocaleFiles(BASE_LOCALE)
  const enList = [...enFiles].sort()
  const report = {
    locales: {},
    summary: {
      missingFiles: 0,
      extraFiles: 0,
      likelyUntranslated: 0,
    },
  }

  for (const locale of TARGET_LOCALES) {
    const localeFiles = loadLocaleFiles(locale)
    const localeList = [...localeFiles].sort()

    const missingFiles = enList.filter((relPath) => !localeFiles.has(relPath))
    const extraFiles = localeList.filter((relPath) => !enFiles.has(relPath))

    const commonMarkdownFiles = enList.filter((relPath) => relPath.endsWith('.md') && localeFiles.has(relPath))
    const untranslatedCandidates = []

    for (const relPath of commonMarkdownFiles) {
      const signal = evaluateTranslationSignals(locale, relPath)
      if (signal) {
        untranslatedCandidates.push(signal)
      }
    }

    report.locales[locale] = {
      englishReferenceCount: enList.length,
      localeFileCount: localeList.length,
      missingFiles,
      extraFiles,
      untranslatedCandidates,
    }

    report.summary.missingFiles += missingFiles.length
    report.summary.extraFiles += extraFiles.length
    report.summary.likelyUntranslated += untranslatedCandidates.length
  }

  return report
}

function printReport(report) {
  console.log('Translation Audit (en -> ja, ru, fr)')
  console.log('=====================================')

  for (const locale of TARGET_LOCALES) {
    const details = report.locales[locale]
    console.log(`\n[${locale}]`) 
    console.log(`- Reference files: ${details.englishReferenceCount}`)
    console.log(`- Locale files:    ${details.localeFileCount}`)
    console.log(`- Missing:         ${details.missingFiles.length}`)
    console.log(`- Extra:           ${details.extraFiles.length}`)
    console.log(`- Likely untranslated: ${details.untranslatedCandidates.length}`)

    if (details.missingFiles.length > 0) {
      console.log('  Missing files:')
      for (const relPath of details.missingFiles) {
        console.log(`    - ${relPath}`)
      }
    }

    if (details.extraFiles.length > 0) {
      console.log('  Extra files (not in en):')
      for (const relPath of details.extraFiles) {
        console.log(`    - ${relPath}`)
      }
    }

    if (details.untranslatedCandidates.length > 0) {
      console.log('  Likely untranslated pages:')
      for (const entry of details.untranslatedCandidates) {
        const exactNote = entry.exactCopy ? ' exact-copy' : ''
        console.log(
          `    - ${entry.path} (overlap=${entry.overlap}, jaccard=${entry.jaccard}, scriptChars=${entry.localeScriptCount}, latinWords=${entry.latinWordCount}${exactNote})`,
        )
      }
    }
  }

  console.log('\nSummary')
  console.log('-------')
  console.log(`Missing files: ${report.summary.missingFiles}`)
  console.log(`Extra files: ${report.summary.extraFiles}`)
  console.log(`Likely untranslated pages: ${report.summary.likelyUntranslated}`)
}

const report = runAudit()
printReport(report)

const hasFailures = report.summary.missingFiles > 0 || report.summary.likelyUntranslated > 0
process.exitCode = hasFailures ? 1 : 0
