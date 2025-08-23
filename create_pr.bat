@echo off
echo Creating GitHub PR...

echo Please replace YOUR_GITHUB_TOKEN with your actual token in this file
echo Then run: curl -X POST -H "Authorization: token YOUR_GITHUB_TOKEN" -H "Accept: application/vnd.github.v3+json" -H "Content-Type: application/json" -d @pr_data.json https://api.github.com/repos/kimlam2010/OHT_V2/pulls

echo.
echo PR creation template ready!
pause
