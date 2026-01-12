# Instructions for Creating Ezine Metadata

You are tasked with creating rich, structured metadata for underground BBS-era ezines from the 1980s-2000s. This metadata will help preserve the historical context and make the archive more discoverable.

## Your Task

Create a YAML metadata file for a single ezine by analyzing its content and structure. The metadata should be **factual, neutral, and historically accurate**.

## Input

You will be given:
- **Ezine slug**: The folder name (e.g., `phrack`, `Northern_Phun_Co`, `40hex`)
- **Access to files**: You can read files from `../archives/{ezine_slug}/`

## Output

Create a file named `meta.yaml` in `src/pages/ezine/{ezine_slug}/` following the structure in `TEMPLATE.yaml`.

## Process

### 1. Initial Exploration (5-10 minutes)

**Start with these files** (if they exist):
- `README`, `README.TXT`, `INFO`, `00INDEX`
- First issue's NFO, DIZ, or TXT files
- Any file with "about", "intro", or "masthead" in the name

**Look for:**
- Ezine name and official branding
- Author handles and group names
- Publication dates/years
- Mission statement or description

### 2. Sample Representative Content (10-15 minutes)

**Read 3-5 files from different issues** to understand:
- Topics covered (hacking, phreaking, viruses, culture, etc.)
- Technical depth (beginner tutorials vs advanced techniques)
- Writing style (formal, casual, humorous, aggressive)
- Recurring sections or columns
- Language(s) used

**Diverse sampling strategy:**
- Pick files from early, middle, and late issues
- Include different file types (articles, code, tutorials, news)
- Check multiple authors if possible

### 3. Fill Out Metadata

Use `TEMPLATE.yaml` as your guide. Key sections:

#### Basic Information
- `slug`: Must exactly match the folder name
- `name`: Official name (check mastheads, not folder name)
- `description`: 1-2 sentences capturing the essence

#### Historical Context
- `years_active`: Determine from issue dates/headers
- `publication_frequency`: Infer from issue numbering/dates
- `authors`: List handles found in credits
- `groups`: Any affiliated hacker/cracker groups
- `origin`: Geographic origin (check phone numbers, language, cultural references)

#### Content Classification
- `languages`: ISO codes (en, fr, es, de, nl)
- `topics`: Select all that apply from the template list
- `technical_level`: Assess from content complexity

#### Significance & Influence
- `significance`: Why does this ezine matter? What made it unique?
  - First to cover certain topics?
  - Influential in the scene?
  - Notable for quality/quantity?
  - Associated with famous hackers/groups?
- `influenced_by`: Other ezines mentioned or referenced
- `related_ezines`: Contemporary or similar publications

#### Content Summary
- `typical_content`: List recurring themes/sections (be specific!)
- `highlights`: Notable articles (optional, only if truly significant)

#### Quality Notes
- `preservation_notes`: Missing issues? Formatting problems? Incomplete?
- `content_warnings`: Explicit language? Illegal techniques? Offensive content?

### 4. Metadata Fields

```yaml
metadata_created: "YYYY-MM-DD"  # Today's date
metadata_updated: "YYYY-MM-DD"  # Same as created initially
metadata_version: "1.0"
metadata_source: "llm_generated"
metadata_reviewed: false
```

## Important Guidelines

### DO:
✅ Be factual and cite what you observe in the files
✅ Use "unknown" rather than guessing
✅ Preserve historical context neutrally
✅ Note technical innovations or "firsts"
✅ Mention connections to other ezines/groups
✅ Include content warnings where appropriate
✅ Check issue dates in filenames and headers
✅ Look for author credits in each issue (they change!)

### DON'T:
❌ Moralize or judge the content
❌ Sensationalize illegal activities
❌ Guess at information you can't verify
❌ Assume modern values apply to historical content
❌ Skip reading actual files (don't just use filenames)
❌ Ignore multilingual content
❌ Copy text verbatim (summarize in your own words)
❌ Add personal opinions

## Special Cases

### Non-English Ezines
- Specify language(s) in `languages` field
- Add English translation in `translations.en` section
- Note if bilingual (many Quebec/European zines mixed languages)

### One-Shot or Incomplete Collections
- Note in `preservation_notes` if only 1-2 issues exist
- Indicate if this appears to be a complete archive or partial

### Offensive Content
- Be honest in `content_warnings` without endorsing
- Context: "Contains racist language typical of 1990s BBS culture"

### Technical/Code-Heavy Zines
- Note primary programming languages if relevant
- Mention if source code is included
- Specify platforms (DOS, Unix, Amiga, etc.)

## Quality Checks

Before submitting, verify:
- [ ] Slug matches folder name exactly
- [ ] Years active match file dates
- [ ] Topics list is comprehensive
- [ ] Description is concise (1-2 sentences)
- [ ] Significance paragraph explains historical importance
- [ ] Content warnings are appropriate
- [ ] No spelling errors in factual information
- [ ] YAML syntax is valid (no tabs, proper indentation)

## Example Workflow

```bash
# 1. Navigate to ezine folder
cd src/pages/ezine/phrack/

# 2. List all issues in archive
ls ../../../../../archives/phrack/

# 3. Check for info files
cat ../../../../../archives/phrack/README.txt

# 4. Read first issue
cat ../../../../../archives/phrack/phrack01/phrack-01.txt

# 5. Sample from middle
cat ../../../../../archives/phrack/phrack25/some-article.txt

# 6. Check recent issue
cat ../../../../../archives/phrack/phrack50/intro.txt

# 7. Create metadata file
cp ../../../../metadata/TEMPLATE.yaml meta.yaml
# ... edit with findings ...
```

## Time Estimate

- **Simple ezine** (1-10 issues, clear structure): 15-20 minutes
- **Medium ezine** (10-50 issues, mixed content): 20-30 minutes
- **Complex ezine** (50+ issues, multilingual, unclear): 30-45 minutes

## Need Help?

If you encounter issues:
- **Missing files**: Note in `preservation_notes`
- **Unclear dates**: Use approximate ranges ("early 1990s")
- **Conflicting info**: Note the discrepancy in `preservation_notes`
- **Non-standard format**: Do your best, note oddities

## Final Note

This metadata will help researchers, archivists, and curious users understand the historical significance of these underground publications. **Accuracy and neutrality are more important than completeness.** It's better to leave fields empty than to guess.

Your work helps preserve digital history. Thank you! 🏛️
