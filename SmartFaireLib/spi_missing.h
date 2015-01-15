// Copyright(c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the BSD 2 - Clause License.
// See License.txt in the project root for license information.

#include "spi.h"
#define SPI_CONTINUE 0
#define SPI_LAST     1

// Namespace to replace the missing SPI calls
namespace SPI_Missing
{
    // Calls SPI.transfer and toggles the slaveSelectPin to match.
    inline int transfer(int slaveSelectPin, unsigned char val, int transferMode)
    {
        int ret;

        digitalWrite(slaveSelectPin, LOW);

        ret = SPI.transfer(val);

        if (transferMode != SPI_CONTINUE)
        {
            digitalWrite(slaveSelectPin, HIGH);
        }

        return ret;
    }

    // 8-bit transfers
    inline int transfer(int slaveSelectPin, unsigned char val)
    {
        return transfer(slaveSelectPin, val, SPI_LAST);
    }

    //ULONG pinSelectBitmask;
}