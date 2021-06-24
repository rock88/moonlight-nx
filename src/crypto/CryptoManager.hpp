#include <stdio.h>
#pragma once

#if defined(USE_OPENSSL)

#include "OpenSSLCryptoManager.hpp"
#define CryptoManager OpenSSLCryptoManager

#elif defined(USE_MBEDTLS)

#include "MbedTLSCryptoManager.hpp"
#define CryptoManager MbedTLSCryptoManager

#else
#error Select crypto!
#endif
