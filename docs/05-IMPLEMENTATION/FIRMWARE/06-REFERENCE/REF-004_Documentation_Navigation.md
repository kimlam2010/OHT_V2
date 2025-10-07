# OHT-50 Firmware Documentation Index

Complete documentation for OHT-50 firmware development, deployment, and operations.

## Documentation Location

**Main Documentation:** `docs/` → `/home/orangepi/Desktop/OHT_V2/docs/05-IMPLEMENTATION/FIRMWARE`

The `docs/` directory is a symbolic link to the main project documentation.

## Documentation Structure

```
docs/ (symlink to OHT_V2/docs/05-IMPLEMENTATION/FIRMWARE/)
├── 00-OVERVIEW/            # Project overview and introduction
├── 01-ARCHITECTURE/        # Architecture documentation
├── 02-IMPLEMENTATION/      # Implementation guides
├── 03-DEPLOYMENT/          # Deployment procedures
├── 04-OPERATIONS/          # Operations and maintenance
├── api/                    # API documentation
├── architecture/           # Architecture diagrams
├── safety/                 # Safety documentation
└── user_guide/             # User guides
```

## Key Documents

### Overview
- **README.md** - Main project README
- **00-OVERVIEW/** - Project introduction and context

### Architecture
- **FW_APP_SAFETY_ARCHITECTURE.md** - Safety system architecture
- **01-ARCHITECTURE/** - System architecture documentation
- **architecture/** - Architecture diagrams and designs

### Implementation
- **FW_APP_IMPLEMENTATION_PLAN.md** - Implementation plan
- **CTO_REFACTOR_ORDER.md** - Refactoring roadmap
- **02-IMPLEMENTATION/** - Implementation guides

### Deployment
- **03-DEPLOYMENT/** - Deployment procedures and guides

### Operations
- **04-OPERATIONS/** - Operations and maintenance documentation

### API
- **api/** - API specifications and documentation

### Safety
- **safety/** - Safety system documentation and compliance

### User Guide
- **user_guide/** - End-user documentation

## Local Documentation

### In This Repository

#### Root Level
- `README.md` - Repository overview
- `CODE_STRUCTURE.md` - Code organization
- `DOCUMENTATION.md` - This file
- `.gitignore` - Git ignore patterns

#### Source Code
- `src/README.md` - Source code structure (to be created)
- `include/README.md` - Header files organization
- `tests/README.md` - Test suite documentation

#### Scripts
- `scripts/build/README.md` - Build scripts
- `scripts/deploy/README.md` - Deploy scripts
- `scripts/test/README.md` - Test scripts
- `scripts/rs485/README.md` - RS485 scripts
- `scripts/lidar/README.md` - LiDAR scripts
- `scripts/safety/README.md` - Safety scripts

## Quick Reference

### For Developers

**Getting Started:**
1. Read `README.md`
2. Review `CODE_STRUCTURE.md`
3. Check `docs/01-ARCHITECTURE/`
4. Read implementation guides in `docs/02-IMPLEMENTATION/`

**Architecture Understanding:**
1. `docs/01-ARCHITECTURE/` - System architecture
2. `FW_APP_SAFETY_ARCHITECTURE.md` - Safety architecture
3. `architecture/` - Architecture diagrams

**Implementation:**
1. `FW_APP_IMPLEMENTATION_PLAN.md` - Implementation plan
2. `CTO_REFACTOR_ORDER.md` - Refactoring guide
3. `docs/02-IMPLEMENTATION/` - Implementation details

**Testing:**
1. `tests/README.md` - Test suite documentation
2. Test scripts in `scripts/test/`

### For Operators

**Deployment:**
1. `docs/03-DEPLOYMENT/` - Deployment procedures
2. Deploy scripts in `scripts/deploy/`

**Operations:**
1. `docs/04-OPERATIONS/` - Operations guides
2. `docs/user_guide/` - User guides

**Maintenance:**
1. `docs/04-OPERATIONS/` - Maintenance procedures
2. Safety documentation in `docs/safety/`

### For API Users

**API Documentation:**
1. `docs/api/` - API specifications
2. API test examples in `tests/api/`

## Documentation Standards

### Markdown Format
- All documentation in Markdown (.md)
- Follow standard Markdown syntax
- Use code blocks for examples
- Include diagrams where helpful

### Structure
- Clear hierarchy
- Table of contents for long documents
- Cross-references between related documents
- Version information and update dates

### Content
- Clear and concise writing
- Technical accuracy
- Code examples where appropriate
- Diagrams and visualizations

### Maintenance
- Update documentation with code changes
- Review documentation periodically
- Keep version information current
- Mark deprecated content clearly

## Contributing to Documentation

### Adding Documentation
1. Choose appropriate location
2. Follow naming conventions
3. Use Markdown format
4. Add to relevant index
5. Update this file if needed

### Updating Documentation
1. Make changes in source location
2. Update version/date information
3. Review related documents
4. Update indexes if structure changes

### Documentation Review
- Technical accuracy
- Clarity and readability
- Completeness
- Up-to-date information
- Proper formatting

## Documentation Tools

### Viewing
- **Markdown viewers:** VSCode, Typora, Grip
- **Online:** GitHub, GitLab
- **CLI:** pandoc, markdown

### Generating
- **API docs:** Doxygen (for C code)
- **Diagrams:** Mermaid, PlantUML
- **PDF:** pandoc, LaTeX

### Linting
- **Markdown:** markdownlint
- **Links:** markdown-link-check
- **Spell check:** aspell, hunspell

## Related Files

### Local Documentation Files
```
firmware_new/
├── README.md              # Main README
├── CODE_STRUCTURE.md      # Code organization
├── DOCUMENTATION.md       # This file
├── src/README.md          # Source documentation (TBD)
├── include/README.md      # Header documentation
├── tests/README.md        # Test documentation
└── scripts/*/README.md    # Script documentation
```

### External Documentation
- Project-wide docs: `/home/orangepi/Desktop/OHT_V2/docs/`
- Firmware docs: `/home/orangepi/Desktop/OHT_V2/docs/05-IMPLEMENTATION/FIRMWARE/`

## Documentation Categories

### Technical Documentation
- Architecture designs
- API specifications
- Implementation guides
- Test documentation

### User Documentation
- User guides
- Operations manuals
- Deployment procedures
- Troubleshooting guides

### Process Documentation
- Development workflow
- Review procedures
- Testing strategy
- Release process

### Compliance Documentation
- Safety documentation
- Standards compliance
- Certification records
- Audit trails

## Search and Navigation

### Finding Documentation

**By Topic:**
```bash
# Search all documentation
grep -r "search_term" docs/

# Search local docs only
grep -r "search_term" *.md */README.md
```

**By File Type:**
```bash
# Find all markdown files
find docs/ -name "*.md"

# Find specific document
find docs/ -name "*safety*"
```

### Navigation Tips
1. Start with README files in each directory
2. Follow cross-references
3. Use table of contents in long documents
4. Check related documents section

## Documentation Maintenance

### Regular Tasks
- [ ] Review and update quarterly
- [ ] Check for broken links monthly
- [ ] Update version information with releases
- [ ] Archive obsolete documentation

### Quality Checks
- [ ] Technical accuracy verified
- [ ] Code examples tested
- [ ] Links functional
- [ ] Formatting correct
- [ ] Up-to-date with code

---

**Last Updated:** 2025-10-07  
**Version:** 1.0  
**Maintained By:** Firmware Team


