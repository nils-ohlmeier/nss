/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file contains prototypes for the public SSL functions.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __sslt_h_
#define __sslt_h_

#include "prtypes.h"

typedef struct SSL3StatisticsStr {
    /* statistics from ssl3_SendClientHello (sch) */
    long sch_sid_cache_hits;
    long sch_sid_cache_misses;
    long sch_sid_cache_not_ok;

    /* statistics from ssl3_HandleServerHello (hsh) */
    long hsh_sid_cache_hits;
    long hsh_sid_cache_misses;
    long hsh_sid_cache_not_ok;

    /* statistics from ssl3_HandleClientHello (hch) */
    long hch_sid_cache_hits;
    long hch_sid_cache_misses;
    long hch_sid_cache_not_ok;

    /* statistics related to stateless resume */
    long sch_sid_stateless_resumes;
    long hsh_sid_stateless_resumes;
    long hch_sid_stateless_resumes;
    long hch_sid_ticket_parse_failures;
} SSL3Statistics;

/* Key Exchange algorithm values */
typedef enum {
    ssl_kea_null = 0,
    ssl_kea_rsa = 1,
    ssl_kea_dh = 2,
    ssl_kea_fortezza = 3, /* deprecated, now unused */
    ssl_kea_ecdh = 4,
    ssl_kea_size /* number of ssl_kea_ algorithms */
} SSLKEAType;

/* The following defines are for backwards compatibility.
** They will be removed in a forthcoming release to reduce namespace pollution.
** programs that use the kt_ symbols should convert to the ssl_kt_ symbols
** soon.
*/
#define kt_null ssl_kea_null
#define kt_rsa ssl_kea_rsa
#define kt_dh ssl_kea_dh
#define kt_fortezza ssl_kea_fortezza /* deprecated, now unused */
#define kt_ecdh ssl_kea_ecdh
#define kt_kea_size ssl_kea_size

/* Values of this enum match the SignatureAlgorithm enum from
 * https://tools.ietf.org/html/rfc5246#section-7.4.1.4.1 */
typedef enum {
    ssl_sign_null = 0, /* "anonymous" in TLS */
    ssl_sign_rsa = 1,
    ssl_sign_dsa = 2,
    ssl_sign_ecdsa = 3,
    ssl_sign_psk = 4
} SSLSignType;

/* Values of this enum match the HashAlgorithm enum from
 * https://tools.ietf.org/html/rfc5246#section-7.4.1.4.1 */
typedef enum {
    /* ssl_hash_none is used internally to mean the pre-1.2 combination of MD5
     * and SHA1. The other values are only used in TLS 1.2. */
    ssl_hash_none = 0,
    ssl_hash_md5 = 1,
    ssl_hash_sha1 = 2,
    ssl_hash_sha224 = 3,
    ssl_hash_sha256 = 4,
    ssl_hash_sha384 = 5,
    ssl_hash_sha512 = 6
} SSLHashType;

typedef struct SSLSignatureAndHashAlgStr {
    SSLHashType hashAlg;
    SSLSignType sigAlg;
} SSLSignatureAndHashAlg;

typedef enum {
    ssl_auth_null = 0,
    ssl_auth_rsa = 1,
    ssl_auth_dsa = 2,
    ssl_auth_kea = 3,
    ssl_auth_ecdsa = 4,
    ssl_auth_psk = 5         /* Used for both PSK and (EC)DHE-PSK */
} SSLAuthType;

typedef enum {
    ssl_calg_null = 0,
    ssl_calg_rc4 = 1,
    ssl_calg_rc2 = 2,
    ssl_calg_des = 3,
    ssl_calg_3des = 4,
    ssl_calg_idea = 5,
    ssl_calg_fortezza = 6, /* deprecated, now unused */
    ssl_calg_aes = 7,
    ssl_calg_camellia = 8,
    ssl_calg_seed = 9,
    ssl_calg_aes_gcm = 10,
    ssl_calg_chacha20 = 11
} SSLCipherAlgorithm;

typedef enum {
    ssl_mac_null = 0,
    ssl_mac_md5 = 1,
    ssl_mac_sha = 2,
    ssl_hmac_md5 = 3, /* TLS HMAC version of mac_md5 */
    ssl_hmac_sha = 4, /* TLS HMAC version of mac_sha */
    ssl_hmac_sha256 = 5,
    ssl_mac_aead = 6
} SSLMACAlgorithm;

typedef enum {
    ssl_compression_null = 0,
    ssl_compression_deflate = 1 /* RFC 3749 */
} SSLCompressionMethod;

typedef struct SSLChannelInfoStr {
    /* |length| is obsolete. On return, SSL_GetChannelInfo sets |length| to the
     * smaller of the |len| argument and the length of the struct. The caller
     * may ignore |length|. */
    PRUint32 length;
    PRUint16 protocolVersion;
    PRUint16 cipherSuite;

    /* server authentication info */
    PRUint32 authKeyBits;

    /* key exchange algorithm info */
    PRUint32 keaKeyBits;

    /* session info */
    PRUint32 creationTime;    /* seconds since Jan 1, 1970 */
    PRUint32 lastAccessTime;  /* seconds since Jan 1, 1970 */
    PRUint32 expirationTime;  /* seconds since Jan 1, 1970 */
    PRUint32 sessionIDLength; /* up to 32 */
    PRUint8 sessionID[32];

    /* The following fields are added in NSS 3.12.5. */

    /* compression method info */
    const char* compressionMethodName;
    SSLCompressionMethod compressionMethod;

    /* The following fields are added in NSS 3.21.
     * This field only has meaning in TLS < 1.3 and will be set to
     *  PR_FALSE in TLS 1.3.
     */
    PRBool extendedMasterSecretUsed;
} SSLChannelInfo;

/* Preliminary channel info */
#define ssl_preinfo_version (1U << 0)
#define ssl_preinfo_cipher_suite (1U << 1)
#define ssl_preinfo_all (ssl_preinfo_version | ssl_preinfo_cipher_suite)

typedef struct SSLPreliminaryChannelInfoStr {
    /* |length| is obsolete. On return, SSL_GetPreliminaryChannelInfo sets
     * |length| to the smaller of the |len| argument and the length of the
     * struct. The caller may ignore |length|. */
    PRUint32 length;
    /* A bitfield over SSLPreliminaryValueSet that describes which
     * preliminary values are set (see ssl_preinfo_*). */
    PRUint32 valuesSet;
    /* Protocol version: test (valuesSet & ssl_preinfo_version) */
    PRUint16 protocolVersion;
    /* Cipher suite: test (valuesSet & ssl_preinfo_cipher_suite) */
    PRUint16 cipherSuite;
} SSLPreliminaryChannelInfo;

typedef struct SSLCipherSuiteInfoStr {
    /* |length| is obsolete. On return, SSL_GetCipherSuitelInfo sets |length|
     * to the smaller of the |len| argument and the length of the struct. The
     * caller may ignore |length|. */
    PRUint16 length;
    PRUint16 cipherSuite;

    /* Cipher Suite Name */
    const char* cipherSuiteName;

    /* server authentication info */
    const char* authAlgorithmName;
    SSLAuthType authAlgorithm;

    /* key exchange algorithm info */
    const char* keaTypeName;
    SSLKEAType keaType;

    /* symmetric encryption info */
    const char* symCipherName;
    SSLCipherAlgorithm symCipher;
    PRUint16 symKeyBits;
    PRUint16 symKeySpace;
    PRUint16 effectiveKeyBits;

    /* MAC info */
    /* AEAD ciphers don't have a MAC. For an AEAD cipher, macAlgorithmName
     * is "AEAD", macAlgorithm is ssl_mac_aead, and macBits is the length in
     * bits of the authentication tag. */
    const char* macAlgorithmName;
    SSLMACAlgorithm macAlgorithm;
    PRUint16 macBits;

    PRUintn isFIPS : 1;
    PRUintn isExportable : 1;
    PRUintn nonStandard : 1;
    PRUintn reservedBits : 29;

} SSLCipherSuiteInfo;

typedef enum {
    ssl_variant_stream = 0,
    ssl_variant_datagram = 1
} SSLProtocolVariant;

typedef struct SSLVersionRangeStr {
    PRUint16 min;
    PRUint16 max;
} SSLVersionRange;

typedef enum {
    SSL_sni_host_name = 0,
    SSL_sni_type_total
} SSLSniNameType;

/* Supported extensions. */
/* Update SSL_MAX_EXTENSIONS whenever a new extension type is added. */
typedef enum {
    ssl_server_name_xtn = 0,
    ssl_cert_status_xtn = 5,
#ifndef NSS_DISABLE_ECC
    ssl_elliptic_curves_xtn = 10,
    ssl_ec_point_formats_xtn = 11,
#endif
    ssl_signature_algorithms_xtn = 13,
    ssl_use_srtp_xtn = 14,
    ssl_app_layer_protocol_xtn = 16,
    /* signed_certificate_timestamp extension, RFC 6962 */
    ssl_signed_cert_timestamp_xtn = 18,
    ssl_padding_xtn = 21,
    ssl_extended_master_secret_xtn = 23,
    ssl_session_ticket_xtn = 35,
    ssl_tls13_key_share_xtn = 40, /* unofficial TODO(ekr) */
    ssl_tls13_pre_shared_key_xtn = 41, /* unofficial TODO(ekr) */
    ssl_tls13_early_data_xtn = 42, /* unofficial TODO(ekr) */
    ssl_next_proto_nego_xtn = 13172,
    ssl_renegotiation_info_xtn = 0xff01,
    ssl_tls13_draft_version_xtn = 0xff02 /* experimental number */
} SSLExtensionType;

#define SSL_MAX_EXTENSIONS 16 /* doesn't include ssl_padding_xtn. */

typedef enum {
    ssl_dhe_group_none = 0,
    ssl_ff_dhe_2048_group = 1,
    ssl_ff_dhe_3072_group = 2,
    ssl_ff_dhe_4096_group = 3,
    ssl_ff_dhe_6144_group = 4,
    ssl_ff_dhe_8192_group = 5,
    ssl_dhe_group_max
} SSLDHEGroupType;

#endif /* __sslt_h_ */
