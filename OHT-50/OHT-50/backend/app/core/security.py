"""
Security utilities: basic bearer token verification dependency
"""

from typing import Optional

from fastapi import Depends, Header, HTTPException, Request, status

from app.config import get_settings


def verify_token(authorization: Optional[str] = Header(None), request: Request = None):
    """Basic Bearer token verification.

    - If settings.debug is True, skip verification (for local dev/tests).
    - Otherwise, require "Authorization: Bearer <token>" with a non-empty token.
    """
    settings = get_settings()
    if getattr(settings, "debug", False):
        return

    if not authorization or not authorization.lower().startswith("bearer "):
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Missing bearer token")

    token = authorization.split(" ", 1)[1].strip()
    if not token:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid bearer token")

    # Minimal check only: any non-empty bearer token is accepted for now
    return


