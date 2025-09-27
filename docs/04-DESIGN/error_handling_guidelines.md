# HAL Error Handling Guidelines

## Error Code Standards:
- HAL_STATUS_OK = 0 (success)
- HAL_STATUS_ERROR = -1 (general errors)
- HAL_STATUS_INVALID_PARAMETER = -2 (parameter errors)

## Usage Rules:
1. **Success cases**: return HAL_STATUS_OK (0)
2. **General errors**: return HAL_STATUS_ERROR (-1)  
3. **Parameter errors**: return HAL_STATUS_INVALID_PARAMETER (-2)
4. **Safety violations**: return HAL_STATUS_ERROR (-1)

## Examples:
- **Uninitialized system**: HAL_STATUS_ERROR (-1)
- **Invalid parameters**: HAL_STATUS_INVALID_PARAMETER (-2)
- **Hardware failures**: HAL_STATUS_ERROR (-1)
- **Safety violations**: HAL_STATUS_ERROR (-1)

## Current Issue Analysis:
The E-Stop tests are failing because:
1. **Success cases** return `HAL_STATUS_ERROR` (-1) instead of `HAL_STATUS_OK` (0)
2. **Error cases** return `HAL_STATUS_OK` (0) instead of `HAL_STATUS_ERROR` (-1)

## Root Cause:
The error handling logic is inverted or there's a deeper issue with the function implementations.

## Required Action:
Fix the function implementations to return correct status codes based on actual success/failure conditions.
