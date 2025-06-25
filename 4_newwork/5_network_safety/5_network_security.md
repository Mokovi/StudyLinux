# 五、网络安全与加密基础

## 目录
- [5.1 网络安全概述](#51-网络安全概述)
- [5.2 SSL/TLS 协议](#52-ssltls-协议)
- [5.3 对称加密技术](#53-对称加密技术)
- [5.4 非对称加密技术](#54-非对称加密技术)
- [5.5 数字证书与PKI](#55-数字证书与pki)
- [5.6 OpenSSL 编程实践](#56-openssl-编程实践)
- [5.7 综合实例](#57-综合实例)

---

## 5.1 网络安全概述

网络安全是网络编程中不可或缺的重要组成部分，涉及数据保密性、完整性、可用性和身份认证等多个方面。

#### 安全威胁
- **窃听攻击**：攻击者截获网络通信数据
- **篡改攻击**：攻击者修改传输中的数据
- **重放攻击**：攻击者重复发送截获的数据包
- **中间人攻击**：攻击者冒充通信双方进行欺骗
- **拒绝服务攻击**：攻击者消耗系统资源导致服务不可用

#### 安全目标
- **保密性（Confidentiality）**：确保信息不被未授权访问
- **完整性（Integrity）**：确保信息在传输过程中不被篡改
- **可用性（Availability）**：确保授权用户能够正常访问服务
- **身份认证（Authentication）**：验证通信双方的身份
- **不可否认性（Non-repudiation）**：防止通信方否认已进行的操作

#### 安全技术
- **加密技术**：对称加密、非对称加密、哈希函数
- **安全协议**：SSL/TLS、SSH、IPSec
- **身份认证**：数字证书、公钥基础设施（PKI）
- **访问控制**：防火墙、入侵检测系统（IDS）

#### 总结
网络安全是网络应用的基础保障，需要综合运用多种技术手段构建安全防护体系。

---

## 5.2 SSL/TLS 协议

SSL（Secure Sockets Layer）和TLS（Transport Layer Security）是网络安全通信的标准协议，为网络通信提供加密、身份认证和数据完整性保护。

#### 协议版本
- **SSL 1.0**：未发布
- **SSL 2.0**：1995年发布，存在安全漏洞
- **SSL 3.0**：1996年发布，已被TLS替代
- **TLS 1.0**：1999年发布，基于SSL 3.0
- **TLS 1.1**：2006年发布
- **TLS 1.2**：2008年发布，广泛使用
- **TLS 1.3**：2018年发布，最新标准

#### 协议特性
- **加密通信**：使用对称加密算法保护数据传输
- **身份认证**：通过数字证书验证服务器身份
- **完整性保护**：使用消息认证码（MAC）防止数据篡改
- **前向安全性**：即使密钥泄露，之前的通信仍然安全

#### 握手过程
```c
// TLS握手流程
客户端 → ClientHello → 服务端
客户端 ← ServerHello ← 服务端
客户端 ← Certificate ← 服务端
客户端 ← ServerKeyExchange ← 服务端
客户端 ← ServerHelloDone ← 服务端
客户端 → ClientKeyExchange → 服务端
客户端 → ChangeCipherSpec → 服务端
客户端 → Finished → 服务端
客户端 ← ChangeCipherSpec ← 服务端
客户端 ← Finished ← 服务端
```

#### 总结
SSL/TLS协议为网络通信提供了强大的安全保障，是现代网络安全的基础。

---

## 5.3 对称加密技术

对称加密使用相同的密钥进行加密和解密，具有加密速度快、效率高的特点。

#### 常用算法
- **AES（Advanced Encryption Standard）**：美国国家标准，广泛使用
- **DES（Data Encryption Standard）**：已过时，不推荐使用
- **3DES（Triple DES）**：DES的三重加密，安全性提高但效率降低
- **ChaCha20**：Google开发的流密码，性能优秀

#### AES算法
```c
// AES加密示例
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <string.h>

void aes_encrypt_example() {
    AES_KEY aes_key;
    unsigned char key[32]; // 256位密钥
    unsigned char iv[16];  // 初始化向量
    unsigned char plaintext[64] = "Hello, AES encryption!";
    unsigned char ciphertext[64];
    
    // 生成随机密钥和IV
    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));
    
    // 设置加密密钥
    AES_set_encrypt_key(key, 256, &aes_key);
    
    // 加密数据
    AES_cbc_encrypt(plaintext, ciphertext, strlen((char*)plaintext), 
                   &aes_key, iv, AES_ENCRYPT);
    
    printf("加密完成\n");
}
```

#### 密钥管理
- **密钥生成**：使用密码学安全的随机数生成器
- **密钥分发**：通过安全通道传输密钥
- **密钥存储**：安全存储密钥，防止泄露
- **密钥更新**：定期更换密钥

#### 总结
对称加密适合大量数据的快速加密，但密钥管理是其主要挑战。

---

## 5.4 非对称加密技术

非对称加密使用公钥和私钥对，公钥用于加密，私钥用于解密，解决了密钥分发问题。

#### 常用算法
- **RSA**：最广泛使用的非对称加密算法
- **ECC（Elliptic Curve Cryptography）**：基于椭圆曲线的加密算法
- **DSA（Digital Signature Algorithm）**：数字签名算法
- **ElGamal**：基于离散对数问题的加密算法

#### RSA算法
```c
// RSA加密示例
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

void rsa_encrypt_example() {
    RSA *rsa = RSA_new();
    BIGNUM *e = BN_new();
    unsigned char plaintext[] = "Hello, RSA!";
    unsigned char ciphertext[256];
    int ciphertext_len;
    
    // 生成RSA密钥对
    BN_set_word(e, RSA_F4); // 使用65537作为公钥指数
    RSA_generate_key_ex(rsa, 2048, e, NULL);
    
    // 使用公钥加密
    ciphertext_len = RSA_public_encrypt(strlen((char*)plaintext), 
                                       plaintext, ciphertext, rsa, RSA_PKCS1_PADDING);
    
    if (ciphertext_len == -1) {
        printf("RSA加密失败\n");
    } else {
        printf("RSA加密成功，密文长度: %d\n", ciphertext_len);
    }
    
    // 清理资源
    RSA_free(rsa);
    BN_free(e);
}
```

#### 数字签名
```c
// RSA数字签名示例
void rsa_sign_example() {
    RSA *rsa = RSA_new();
    BIGNUM *e = BN_new();
    unsigned char message[] = "Hello, Digital Signature!";
    unsigned char signature[256];
    unsigned int signature_len;
    
    // 生成RSA密钥对
    BN_set_word(e, RSA_F4);
    RSA_generate_key_ex(rsa, 2048, e, NULL);
    
    // 使用私钥签名
    signature_len = RSA_sign(NID_sha256, message, strlen((char*)message), 
                            signature, &signature_len, rsa);
    
    if (signature_len == 0) {
        printf("RSA签名失败\n");
    } else {
        printf("RSA签名成功，签名长度: %d\n", signature_len);
        
        // 使用公钥验证签名
        int verify_result = RSA_verify(NID_sha256, message, strlen((char*)message), 
                                      signature, signature_len, rsa);
        if (verify_result == 1) {
            printf("签名验证成功\n");
        } else {
            printf("签名验证失败\n");
        }
    }
    
    RSA_free(rsa);
    BN_free(e);
}
```

#### 总结
非对称加密解决了密钥分发问题，但计算开销较大，通常用于密钥交换和数字签名。

---

## 5.5 数字证书与PKI

数字证书是公钥基础设施（PKI）的核心组件，用于身份认证和公钥分发。

#### 证书结构
```c
// X.509证书结构
Certificate
├── Version
├── Serial Number
├── Signature Algorithm ID
├── Issuer Name
├── Validity Period
│   ├── Not Before
│   └── Not After
├── Subject Name
├── Subject Public Key Info
│   ├── Public Key Algorithm
│   └── Subject Public Key
├── Issuer Unique Identifier (optional)
├── Subject Unique Identifier (optional)
├── Extensions (optional)
└── Certificate Signature Algorithm
└── Certificate Signature
```

#### 证书类型
- **CA证书**：证书颁发机构的根证书
- **服务器证书**：用于Web服务器身份认证
- **客户端证书**：用于客户端身份认证
- **代码签名证书**：用于软件代码签名

#### 证书验证
```c
// 证书验证示例
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

int verify_certificate_example() {
    X509 *cert = NULL;
    X509_STORE *store = NULL;
    X509_STORE_CTX *ctx = NULL;
    FILE *fp;
    int result;
    
    // 加载证书
    fp = fopen("server.crt", "r");
    if (!fp) {
        printf("无法打开证书文件\n");
        return -1;
    }
    
    cert = PEM_read_X509(fp, NULL, NULL, NULL);
    fclose(fp);
    
    if (!cert) {
        printf("无法读取证书\n");
        return -1;
    }
    
    // 创建证书存储
    store = X509_STORE_new();
    X509_STORE_set_default_paths(store);
    
    // 创建验证上下文
    ctx = X509_STORE_CTX_new();
    X509_STORE_CTX_init(ctx, store, cert, NULL);
    
    // 验证证书
    result = X509_verify_cert(ctx);
    
    if (result == 1) {
        printf("证书验证成功\n");
    } else {
        printf("证书验证失败: %s\n", X509_verify_cert_error_string(X509_STORE_CTX_get_error(ctx)));
    }
    
    // 清理资源
    X509_STORE_CTX_free(ctx);
    X509_STORE_free(store);
    X509_free(cert);
    
    return result;
}
```

#### PKI组件
- **证书颁发机构（CA）**：负责签发和管理数字证书
- **注册机构（RA）**：负责验证申请者身份
- **证书存储库**：存储和分发证书
- **证书撤销列表（CRL）**：存储已撤销的证书

#### 总结
数字证书和PKI为网络通信提供了可靠的身份认证机制，是网络安全的重要基础。

---

## 5.6 OpenSSL 编程实践

OpenSSL是一个开源的SSL/TLS工具包，提供了丰富的密码学功能。

#### 初始化OpenSSL
```c
#include <openssl/ssl.h>
#include <openssl/err.h>

void init_openssl() {
    // 初始化SSL库
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    printf("OpenSSL初始化完成\n");
}

void cleanup_openssl() {
    // 清理OpenSSL资源
    EVP_cleanup();
    ERR_free_strings();
}
```

#### SSL/TLS服务器
```c
// SSL/TLS服务器示例
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8443
#define BUFFER_SIZE 4096

SSL_CTX* create_ssl_context() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    // 设置证书和私钥
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    return ctx;
}

void handle_ssl_client(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    int bytes;
    
    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
        buffer[bytes] = '\0';
        printf("收到: %s", buffer);
        
        // 回显数据
        SSL_write(ssl, buffer, bytes);
    }
    
    SSL_shutdown(ssl);
    SSL_free(ssl);
}

int main() {
    int sockfd, clientfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    SSL_CTX *ctx;
    SSL *ssl;
    
    // 初始化OpenSSL
    init_openssl();
    
    // 创建SSL上下文
    ctx = create_ssl_context();
    if (!ctx) {
        printf("创建SSL上下文失败\n");
        return 1;
    }
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(sockfd, 5);
    
    printf("SSL服务器启动，监听端口 %d\n", PORT);
    
    while (1) {
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (clientfd < 0) {
            perror("accept failed");
            continue;
        }
        
        // 创建SSL连接
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, clientfd);
        
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(clientfd);
            continue;
        }
        
        printf("SSL连接建立成功\n");
        handle_ssl_client(ssl);
        close(clientfd);
    }
    
    SSL_CTX_free(ctx);
    cleanup_openssl();
    close(sockfd);
    return 0;
}
```

#### SSL/TLS客户端
```c
// SSL/TLS客户端示例
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8443
#define BUFFER_SIZE 4096

SSL_CTX* create_ssl_context() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    
    // 设置CA证书（可选）
    if (SSL_CTX_load_verify_locations(ctx, "ca.crt", NULL) <= 0) {
        printf("警告: 无法加载CA证书\n");
    }
    
    return ctx;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    SSL_CTX *ctx;
    SSL *ssl;
    char buffer[BUFFER_SIZE];
    
    // 初始化OpenSSL
    init_openssl();
    
    // 创建SSL上下文
    ctx = create_ssl_context();
    if (!ctx) {
        printf("创建SSL上下文失败\n");
        return 1;
    }
    
    // 创建套接字并连接
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("connect failed");
        return 1;
    }
    
    // 创建SSL连接
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(sockfd);
        return 1;
    }
    
    printf("SSL连接建立成功\n");
    printf("使用的加密套件: %s\n", SSL_get_cipher(ssl));
    
    // 发送数据
    strcpy(buffer, "Hello, SSL Server!\n");
    SSL_write(ssl, buffer, strlen(buffer));
    
    // 接收数据
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("收到: %s", buffer);
    }
    
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    
    return 0;
}
```

#### 总结
OpenSSL提供了完整的SSL/TLS实现，是网络安全编程的重要工具。

---

## 5.7 综合实例

### 实现目标
创建一个支持HTTPS的简单Web服务器，包含证书生成、SSL/TLS通信和基本的安全功能。

### 涉及知识点
- SSL/TLS协议实现
- 数字证书管理
- 对称和非对称加密
- OpenSSL编程接口
- 网络安全最佳实践

### 代码实现

#### 证书生成工具
```c
// 生成自签名证书
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

void generate_self_signed_cert() {
    X509 *cert = X509_new();
    EVP_PKEY *pkey = EVP_PKEY_new();
    RSA *rsa = RSA_new();
    BIGNUM *e = BN_new();
    X509_NAME *name = X509_NAME_new();
    
    // 生成RSA密钥对
    BN_set_word(e, RSA_F4);
    RSA_generate_key_ex(rsa, 2048, e, NULL);
    EVP_PKEY_assign_RSA(pkey, rsa);
    
    // 设置证书信息
    X509_set_version(cert, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
    X509_gmtime_adj(X509_get_notBefore(cert), 0);
    X509_gmtime_adj(X509_get_notAfter(cert), 365 * 24 * 3600); // 1年有效期
    
    X509_set_pubkey(cert, pkey);
    
    // 设置证书主题
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)"CN", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, (unsigned char*)"Beijing", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, (unsigned char*)"Beijing", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)"My Company", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "OU", MBSTRING_ASC, (unsigned char*)"IT Department", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)"localhost", -1, -1, 0);
    
    X509_set_subject_name(cert, name);
    X509_set_issuer_name(cert, name);
    
    // 添加扩展
    X509V3_CTX ctx;
    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
    
    // 添加主题备用名称
    X509_EXTENSION *ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_subject_alt_name, 
                                              "DNS:localhost,IP:127.0.0.1");
    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
    
    // 添加密钥用途
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_key_usage, 
                              "digitalSignature,keyEncipherment");
    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
    
    // 添加扩展密钥用途
    ext = X509V3_EXT_conf_nid(NULL, &ctx, NID_ext_key_usage, 
                              "serverAuth,clientAuth");
    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
    
    // 使用私钥签名证书
    X509_sign(cert, pkey, EVP_sha256());
    
    // 保存证书和私钥
    FILE *fp = fopen("server.crt", "w");
    PEM_write_X509(fp, cert);
    fclose(fp);
    
    fp = fopen("server.key", "w");
    PEM_write_PrivateKey(fp, pkey, NULL, NULL, 0, NULL, NULL);
    fclose(fp);
    
    printf("自签名证书生成完成\n");
    printf("证书文件: server.crt\n");
    printf("私钥文件: server.key\n");
    
    // 清理资源
    X509_free(cert);
    EVP_PKEY_free(pkey);
    X509_NAME_free(name);
    BN_free(e);
}
```

#### HTTPS服务器
```c
// 完整的HTTPS服务器
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>

#define PORT 8443
#define BUFFER_SIZE 4096

typedef struct {
    SSL *ssl;
    int clientfd;
} client_info_t;

void *handle_https_client(void *arg) {
    client_info_t *info = (client_info_t *)arg;
    SSL *ssl = info->ssl;
    int clientfd = info->clientfd;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int bytes;
    
    // 读取HTTP请求
    bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("收到HTTP请求:\n%s", buffer);
        
        // 构造HTTP响应
        sprintf(response, 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %zu\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html><head><title>HTTPS Server</title></head>"
                "<body><h1>Welcome to HTTPS Server!</h1>"
                "<p>This is a secure connection.</p>"
                "<p>SSL/TLS Version: %s</p>"
                "<p>Cipher Suite: %s</p>"
                "</body></html>",
                strlen("<html><head><title>HTTPS Server</title></head>"
                       "<body><h1>Welcome to HTTPS Server!</h1>"
                       "<p>This is a secure connection.</p>"
                       "<p>SSL/TLS Version: %s</p>"
                       "<p>Cipher Suite: %s</p>"
                       "</body></html>") + 100,
                SSL_get_version(ssl),
                SSL_get_cipher(ssl));
        
        // 发送HTTP响应
        SSL_write(ssl, response, strlen(response));
    }
    
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(clientfd);
    free(info);
    
    pthread_detach(pthread_self());
    return NULL;
}

int main() {
    int sockfd, clientfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    SSL_CTX *ctx;
    SSL *ssl;
    pthread_t tid;
    
    // 初始化OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    // 创建SSL上下文
    ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    
    // 设置证书和私钥
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    
    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    
    // 验证私钥
    if (!SSL_CTX_check_private_key(ctx)) {
        printf("私钥验证失败\n");
        return 1;
    }
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(sockfd, 5);
    
    printf("HTTPS服务器启动，监听端口 %d\n", PORT);
    printf("访问 https://localhost:%d 测试\n", PORT);
    
    while (1) {
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (clientfd < 0) {
            perror("accept failed");
            continue;
        }
        
        // 创建SSL连接
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, clientfd);
        
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(clientfd);
            continue;
        }
        
        printf("HTTPS连接建立成功\n");
        printf("客户端: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("SSL版本: %s\n", SSL_get_version(ssl));
        printf("加密套件: %s\n", SSL_get_cipher(ssl));
        
        // 创建客户端信息结构
        client_info_t *info = malloc(sizeof(client_info_t));
        info->ssl = ssl;
        info->clientfd = clientfd;
        
        // 创建线程处理客户端
        pthread_create(&tid, NULL, handle_https_client, info);
    }
    
    SSL_CTX_free(ctx);
    EVP_cleanup();
    ERR_free_strings();
    close(sockfd);
    return 0;
}
```

### 运行与调试
```bash
# 生成自签名证书
gcc -o generate_cert generate_cert.c -lssl -lcrypto
./generate_cert

# 编译HTTPS服务器
gcc -o https_server https_server.c -lssl -lcrypto -lpthread

# 运行服务器
./https_server

# 测试（使用浏览器或curl）
curl -k https://localhost:8443
```

### 常见问题与解答
- **Q: 为什么需要自签名证书？**
- A: 自签名证书用于开发和测试环境，生产环境应使用CA签发的证书。

- **Q: 如何提高SSL/TLS安全性？**
- A: 使用强加密套件、禁用弱协议版本、定期更新证书。

- **Q: 如何处理证书验证失败？**
- A: 检查证书链、确保证书未过期、验证主机名匹配。

- **Q: 如何优化SSL/TLS性能？**
- A: 使用会话复用、启用OCSP装订、选择合适的加密套件。

### 参考资料
- 《SSL与TLS》
- 《密码学与网络安全》
- OpenSSL官方文档
- RFC 5246 (TLS 1.2)
- RFC 8446 (TLS 1.3) 