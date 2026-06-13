# Emma 02 Help File Review Progress

## Date: 2026-06-13
## Status: COMPLETE

All 103 .htd files have been reviewed (excluding template).

## Review Sessions

### Session 1 (prior days)
- Reviewed all Computer*, Configuration*, Debugger*, Direct*, Dump*, Faq* files
- 59 files had fixes applied, 19 files had no issues

### Session 2 (2026-06-13)
Reviewed 30 files from GeneralMain onwards:

| # | File | Result |
|---|------|--------|
| 1 | GeneralMain.htd | 1 fix (em → strong) |
| 2 | Main.htd | 5 fixes (broken link, cross-ref, "The" in link, parallel structure, CDP names) |
| 3 | Main1802.htd | Multiple fixes (tt formatting, contraction, CDP names throughout) |
| 4 | MainCommandLine.htd | 6 fixes (h1, commas, tt wrap) |
| 5 | MainComxSb.htd | 4 fixes (unclosed strong, Back-up label, cross-ref) |
| 6 | MainConfigurations.htd | 2 fixes (stray </tt>, github capitalisation) |
| 7 | MainFile.htd | 4 fixes (misplaced ), "Please see") |
| 8 | MainFunctionKeys.htd | No issues |
| 9 | MainKeyMappingDefault.htd | No issues |
| 10 | MainMachineCodeSyntax.htd | No issues |
| 11 | MainPseudoCodeSyntax.htd | No issues |
| 12 | MainSuperBasic.htd | 8 fixes (please, commas, tt/strong formatting) |
| 13 | MainWindow.htd | No issues |
| 14 | MainXMLCodeSyntax.htd | No issues |
| 15 | MenuFile.htd | 1 fix (current → currently) |
| 16 | MenuHelp.htd | No issues |
| 17 | MenuMain.htd | No issues |
| 18 | MenuSettings.htd | 4 fixes (2 broken links, strong wrap, Unselect → Deselect) |
| 19 | ProfilerCodeCoverage.htd | 1 comma fix |
| 20 | ProfilerDeadCode.htd | 1 comma fix |
| 21 | ProfilerHotSpots.htd | No issues |
| 22 | ProfilerLinear.htd | No issues |
| 23 | ProfilerMain.htd | No issues |
| 24 | ProfilerNavigation.htd | No issues |
| 25 | ProfilerTimer.htd | No issues |
| 26 | ProfilerView.htd | No issues |
| 27 | TerminalExternal.htd | No issues |
| 28 | TerminalMain.htd | No issues |
| 29 | TerminalSetup.htd | No issues |
| 30 | TerminalVT100.htd | No issues |

## Cross-cutting fixes (2026-06-13)
1. Formal CDP-prefixed CPU names: Fixed across 16+1 files (ComputerClock, DebuggerChip8, DebuggerMain, DebuggerTrace, DirectCopy, DirectExistingCode, DirectLdl, DirectMain, DirectMarking, DirectMnemonic, FaqElfInstBiosio, FaqElfInstElfOsIo, FaqStudioHomebrew, Main, Main1802, MainConfigurations, MainFile)
2. Opcodes spelling: Fixed in DirectMnemonic.htd (3 occurrences: "Op codes" → "Opcodes", "op codes" → "opcodes", "op code" → "opcode")

## Style guide updates (2026-06-13)
1. Added "Informal chip name check" rule under Naming Conventions
2. Added "Opcodes" rule under Naming Conventions

## Known issues (not fixable from HTML alone)
- External links to www.emma02.hobby-site.com are all unreachable
- MainCommandLine.htd: link inside <pre> block — left as-is per author decision
- "my own" kept as intentional first-person authorial voice
