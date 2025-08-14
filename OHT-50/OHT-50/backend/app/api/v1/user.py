from fastapi import APIRouter, HTTPException

from app.models.user import UserSettings
from app.services.user_settings_service import UserSettingsService


router = APIRouter(tags=["User"])
service = UserSettingsService()


@router.get("/settings/{username}", response_model=UserSettings)
async def get_settings(username: str):
	settings = service.get_user_settings(username)
	if not settings:
		raise HTTPException(status_code=404, detail="User settings not found")
	return settings


@router.put("/settings/{username}", response_model=UserSettings)
async def put_settings(username: str, payload: UserSettings):
	if username != payload.username:
		raise HTTPException(status_code=400, detail="Username mismatch")
	return service.update_user_settings(payload)


