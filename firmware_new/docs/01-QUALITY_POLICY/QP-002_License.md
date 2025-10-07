# 📜 License - OHT-50 Firmware

**Version:** 1.0.1  
**Last Updated:** 2025-10-07

---

## MIT License

Copyright (c) 2025 OHT-50 Development Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---

## 📋 **Third-Party Licenses**

This project includes third-party libraries and components with their own licenses:

### **Unity Test Framework**

- **License:** MIT License
- **Copyright:** (c) 2007-2022 Mike Karlesky, Mark VanderVoord, Greg Williams
- **Website:** https://github.com/ThrowTheSwitch/Unity
- **Location:** `third_party/unity/`

```
MIT License

Copyright (c) 2007-2022 Mike Karlesky, Mark VanderVoord, Greg Williams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### **OpenSSL**

- **License:** Apache License 2.0 and SSLeay License (dual-licensed)
- **Copyright:** The OpenSSL Project
- **Website:** https://www.openssl.org/
- **Usage:** SSL/TLS support (optional)

**Note:** OpenSSL is dynamically linked and not included in this repository.

---

## 🔓 **Open Source Attribution**

This project is built on the shoulders of giants. We acknowledge and thank:

| **Component** | **License** | **Website** |
|---------------|-------------|-------------|
| Unity | MIT | https://github.com/ThrowTheSwitch/Unity |
| OpenSSL | Apache 2.0 | https://www.openssl.org/ |
| pthread | LGPL | Part of glibc |
| CMake | BSD-3-Clause | https://cmake.org/ |

---

## 📝 **Usage Permissions**

### ✅ **You CAN:**

- ✅ Use the software for commercial purposes
- ✅ Modify the software
- ✅ Distribute the software
- ✅ Sublicense the software
- ✅ Use the software privately

### 📋 **You MUST:**

- 📋 Include the license and copyright notice
- 📋 State changes if you modify the code
- 📋 Include the original license in distributions

### ❌ **You CANNOT:**

- ❌ Hold the authors liable
- ❌ Use the authors' names for endorsement without permission

---

## 🛡️ **Warranty Disclaimer**

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

**This means:**

⚠️ No warranty for hardware compatibility  
⚠️ No warranty for production use  
⚠️ No warranty for safety-critical applications  
⚠️ No warranty for any damages or losses

**Use at your own risk.**

---

## 🔐 **Security & Safety**

### **Safety-Critical Applications**

⚠️ **WARNING**: This firmware is designed for industrial automation but has NOT been certified for safety-critical applications such as:

- Medical devices
- Life support systems
- Nuclear facilities
- Aviation systems
- Critical infrastructure

If you plan to use this firmware in safety-critical applications:

1. Conduct thorough safety analysis (FMEA, FTA)
2. Obtain proper certifications (IEC 61508, ISO 26262, etc.)
3. Implement additional safety measures
4. Consult with safety engineers
5. Perform extensive testing and validation

### **Security Considerations**

This firmware includes basic security features but may require additional hardening for production deployment:

- 🔒 Authentication: Basic (consider adding OAuth2/JWT)
- 🔒 Encryption: Optional HTTPS (consider enabling in production)
- 🔒 Input Validation: Basic (review for your use case)
- 🔒 Audit Logging: Basic (consider enhancing for compliance)

See [SECURITY.md](SECURITY.md) for detailed security guidelines.

---

## 🏢 **Commercial Support**

While the software is free and open-source, commercial support is available:

- 🔧 **Custom Development**: Feature development, customization
- 🛠️ **Integration Services**: System integration, deployment
- 🎓 **Training**: On-site training, workshops
- 📞 **Priority Support**: 24/7 support, SLA agreements
- 🔐 **Security Audits**: Professional security assessment

**Contact:** commercial@oht50.com

---

## 🤝 **Contributing**

By contributing to this project, you agree that your contributions will be licensed under the MIT License.

See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines.

---

## 📞 **Contact**

For licensing questions:

- **Email:** legal@oht50.com
- **Website:** https://oht50.com/legal
- **GitHub:** https://github.com/your-org/OHT-50-firmware

---

## 📚 **Additional Resources**

- [MIT License FAQ](https://opensource.org/licenses/MIT)
- [Open Source Initiative](https://opensource.org/)
- [Choose a License](https://choosealicense.com/)

---

**Version:** 1.0.1  
**Last Updated:** 2025-10-07  
**Maintained By:** OHT-50 Legal Team

