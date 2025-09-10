import asyncio
from sqlalchemy import text
from app.core.database import get_db_context
from app.core.security import get_password_hash

NEW_PW = "Admin@123456"

async def main():
    async with get_db_context() as db:
        # Trước khi cập nhật
        cur = await db.execute(text("SELECT id, username, email FROM users WHERE username='admin'"))
        row = cur.fetchone()
        print("Before:", row)

        # CẬP NHẬT MẬT KHẨU (đúng tham số bind)
        await db.execute(
            text("UPDATE users SET password_hash=:ph WHERE username='admin'"),
            {"ph": get_password_hash(NEW_PW)}
        )
        await db.commit()

        # Xác nhận sau cập nhật
        cur = await db.execute(text("SELECT id, username, email FROM users WHERE username='admin'"))
        row = cur.fetchone()
        print("After:", row)
        print("UPDATED_ADMIN_PASSWORD:", NEW_PW)

asyncio.run(main())
