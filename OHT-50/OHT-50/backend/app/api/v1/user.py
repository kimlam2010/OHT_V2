from fastapi import APIRouter, HTTPException
from fastapi import Depends, Query
from app.core.security import verify_token

from app.models.user import UserSettings
from app.services.user_settings_service import UserSettingsService
from app.services.log_service import log_service


router = APIRouter(tags=["User"]) 
service = UserSettingsService()


@router.get("/settings/{username}", response_model=UserSettings, dependencies=[Depends(verify_token)])
async def get_settings(username: str):
	settings = service.get_user_settings(username)
	if not settings:
		raise HTTPException(status_code=404, detail="User settings not found")
	return settings


@router.put("/settings/{username}", response_model=UserSettings, dependencies=[Depends(verify_token)])
async def put_settings(username: str, payload: UserSettings):
	if username != payload.username:
		raise HTTPException(status_code=400, detail="Username mismatch")
	updated = service.update_user_settings(payload)
	# audit
	log_service.add_audit(user=username, action="UPDATE_SETTINGS", target="user.settings", result="OK")
	return updated


