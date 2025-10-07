# 🚀 HƯỚNG DẪN TẠO PULL REQUEST

**Ngày tạo:** $(date +%Y-%m-%d)  
**Branch nguồn:** `firmware/fix-discovery-timeout-api-improvements`  
**Branch đích:** `main`

---

## 📊 TỔNG QUAN

### Trạng thái hiện tại:
- ✅ Branch đã push lên remote
- ✅ Working tree clean
- ⚠️ 5 commits chưa merge vào main
- ⚠️ Nhiều files mới và updates

### Commits chưa merge:
1. `09f4e61` - chore: Commit all remaining files including venv updates
2. `0a68653` - chore: Add debugging and setup scripts  
3. `1f71a85` - chore: Add test scripts and tools updates
4. `6779657` - fix: WiFi AP Stop API không kill processes (#193 #194)
5. `ba6fc77` - fix(firmware): optimize module discovery & enhance API documentation

---

## 🎯 BƯỚC TẠO PR

### Bước 1: Truy cập URL tạo PR
```
https://github.com/kimlam2010/OHT_V2/compare/main...firmware/fix-discovery-timeout-api-improvements
```

### Bước 2: Điền thông tin

#### Title:
```
fix(firmware): Fix discovery timeout & API improvements - Merge to main
```

#### Description:
```markdown
## 🎯 Mục đích
Merge toàn bộ các improvements và bug fixes từ branch firmware/fix-discovery-timeout-api-improvements vào main.

## 📦 Nội dung chính
- ✅ Fix WiFi AP Stop API không kill processes (#193 #194)
- ✅ Optimize module discovery & enhance API documentation
- ✅ Add debugging and setup scripts
- ✅ Add test scripts and tools updates
- ✅ Commit all remaining files including venv updates

## 🔍 Thay đổi quan trọng
- Cập nhật cấu trúc documentation theo ISO standard
- Thêm Quality Policy, Quality Manual, Procedures
- Thêm Work Instructions và Forms/Records
- Cải thiện API documentation
- Fix các vấn đề WiFi AP

## ✅ Checklist
- [x] Code đã được test
- [x] Documentation đã cập nhật
- [x] Không có breaking changes
- [x] Ready to merge

## 📊 Files thay đổi
Xem chi tiết trong tab "Files changed"
```

### Bước 3: Thêm Labels
- `enhancement`
- `documentation`
- `bug fix`
- `firmware`

### Bước 4: Assign Reviewers (nếu có)

### Bước 5: Create Pull Request

---

## 🔍 KIỂM TRA SAU KHI TẠO PR

### CI/CD Checks:
- [ ] Build thành công
- [ ] Tests pass
- [ ] Linting pass
- [ ] No conflicts với main

### Code Review:
- [ ] Có ít nhất 1 reviewer approve
- [ ] Tất cả comments đã được resolve
- [ ] No requested changes còn pending

---

## ✅ MERGE VÀO MAIN

### Khi PR được approve:
1. Click "Merge pull request"
2. Chọn merge strategy:
   - **Merge commit** (khuyến nghị): Giữ nguyên commit history
   - Squash and merge: Gộp tất cả commits thành 1
   - Rebase and merge: Rebase lên main
3. Confirm merge
4. Delete branch sau khi merge (optional)

---

## 📝 SAU KHI MERGE

### Update local repository:
```bash
cd /home/orangepi/Desktop/OHT_V2
git checkout main
git pull origin main
```

### Cleanup local branch (optional):
```bash
git branch -d firmware/fix-discovery-timeout-api-improvements
```

---

## 🆘 TROUBLESHOOTING

### Nếu có conflicts:
1. Update branch với main:
   ```bash
   git checkout firmware/fix-discovery-timeout-api-improvements
   git fetch origin
   git rebase origin/main
   # Resolve conflicts
   git push -f origin firmware/fix-discovery-timeout-api-improvements
   ```

### Nếu CI fails:
1. Xem logs để biết lỗi
2. Fix lỗi trong branch
3. Commit và push
4. PR sẽ tự động update

---

## 📚 REFERENCES

- Repository: https://github.com/kimlam2010/OHT_V2
- Current branch: firmware/fix-discovery-timeout-api-improvements
- Main branch: main

---

**Generated:** $(date)  
**Status:** Ready to create PR

