# OHT-50 Backend Security Setup Guide

## JWT Security Configuration

### Overview
The OHT-50 Backend requires a secure JWT secret for production deployment. This guide explains how to properly configure JWT security.

### Security Requirements

#### Production Environment
- **JWT_SECRET is REQUIRED** - Application will fail to start without it
- Must be a cryptographically secure random string
- Minimum 32 characters (256 bits) recommended
- Must be unique per environment (dev/staging/prod)

#### Testing Environment
- JWT_SECRET is optional when `TESTING=true`
- Fallback secret is automatically used for testing
- No security validation in testing mode

### Setup Instructions

#### 1. Generate JWT Secret

Use the provided script to generate a secure JWT secret:

```bash
python scripts/generate_jwt_secret.py
```

This will output a secure 256-bit random secret.

#### 2. Configure Environment

Add the generated secret to your `.env` file:

```bash
# Security - REQUIRED FOR PRODUCTION
JWT_SECRET=your-generated-secret-here
JWT_ALGORITHM=HS256
JWT_EXPIRY=3600

# Testing (automatically set during tests)
TESTING=false
```

#### 3. Environment-Specific Configuration

**Development:**
```bash
JWT_SECRET=dev-secret-here
TESTING=false
```

**Staging:**
```bash
JWT_SECRET=staging-secret-here
TESTING=false
```

**Production:**
```bash
JWT_SECRET=production-secret-here
TESTING=false
```

### Security Best Practices

#### 1. Secret Management
- **Never commit secrets to version control**
- Use environment variable management systems (AWS Secrets Manager, Azure Key Vault, etc.)
- Rotate secrets regularly (every 90 days recommended)
- Use different secrets for each environment

#### 2. Secret Generation
- Use cryptographically secure random generators
- Minimum 32 characters (256 bits)
- Include letters, numbers, and special characters
- Avoid dictionary words or predictable patterns

#### 3. Deployment Security
- Set JWT_SECRET via environment variables in production
- Use secure secret injection methods
- Monitor for secret exposure in logs
- Implement secret rotation procedures

### Validation

The application validates JWT_SECRET configuration on startup:

#### Production Mode (TESTING=false)
- JWT_SECRET must be set and non-empty
- Application fails to start if missing
- Clear error message provided

#### Testing Mode (TESTING=true)
- JWT_SECRET is optional
- Fallback secret used automatically
- No validation errors

### Error Handling

If JWT_SECRET is missing in production:

```
ValueError: JWT_SECRET must be set via environment variable for production. 
Set JWT_SECRET in your .env file or environment variables.
```

### Troubleshooting

#### Application Won't Start
1. Check if JWT_SECRET is set in environment
2. Verify TESTING environment variable
3. Check .env file configuration
4. Ensure no typos in variable names

#### Authentication Failures
1. Verify JWT_SECRET matches between services
2. Check JWT_ALGORITHM configuration
3. Verify JWT_EXPIRY settings
4. Check system clock synchronization

### Security Checklist

- [ ] JWT_SECRET is set for production
- [ ] Secret is cryptographically secure (256+ bits)
- [ ] Different secrets for each environment
- [ ] Secrets not committed to version control
- [ ] Secret rotation schedule established
- [ ] Secure secret management system in place
- [ ] Monitoring for secret exposure
- [ ] Backup and recovery procedures for secrets

### Related Files

- `app/config.py` - Configuration validation
- `app/core/security.py` - JWT implementation
- `env.example` - Environment template
- `scripts/generate_jwt_secret.py` - Secret generator

### Support

For security-related questions or issues:
1. Check this documentation
2. Review error messages carefully
3. Verify environment configuration
4. Contact the development team

---

**⚠️ IMPORTANT:** This is a security-critical configuration. Ensure all team members understand and follow these guidelines.
