//-----------------------------------------------------------------------------
// Copyright (C) HumbleDeer, Anna V, 2024
// Copyright (C) Proxmark3 contributors. See AUTHORS.md for details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE.txt for the text of the license.
//-----------------------------------------------------------------------------
// Borrows some code, albeit modified, from client/src/cmdlfsecurakey.c
// main code for LF aka SamyRun by Samy Kamkar
//-----------------------------------------------------------------------------

#include "standalone.h" // standalone definitions
#include "proxmark3_arm.h"
#include "appmain.h"
#include "fpgaloader.h"
#include "lfops.h"
#include "util.h"
#include "dbprint.h"
#include "ticks.h"

#include <string.h>       // memcpy
#include <ctype.h>        // tolower
#include "commonutil.h"   // ARRAYLEN

void ModInfo(void) {
    DbpString("  LF Securakey standalone simulator (HumbleDeer)");
}

void RunMod(void) {

    StandAloneMode();

    FpgaDownloadAndGo(FPGA_BITSTREAM_LF);

    Dbprintf(">>  LF Securakey Sim started  <<");

#define STATE_SIM 1

    for (;;) {

        WDT_HIT();

        // exit from SamyRun,   send a usbcommand.
        if (data_available()) break;

        // Was our button held down or pressed?
        int button_pressed = BUTTON_HELD(300);
        if (button_pressed != BUTTON_HOLD){
            continue;
        }

        LED_C_ON();   // Simulate
        LED_D_OFF();

        WAIT_BUTTON_RELEASED();

        #define raw_hex = "7FCC0004436002AF34880000" // 12-byte / 24-bit HEX "raw" data (for reference)

        uint8_t raw_bytes[12] = {0x7F, 0xCC, 0x00, 0x04, 0x43, 0x60, 0x02, 0xAF, 0x34, 0x88, 0x00, 0x00}; // The HEX string, but bytes
        int raw_bytes_length = sizeof(raw_bytes);

        Dbprintf("Simulating SecuraKey - raw " _YELLOW_("%s"), sprint_hex_inrow(raw_bytes, sizeof(raw_bytes)));

        uint8_t raw_bits[sizeof(raw_bytes) * 8]; // Make array w/ size of raw[] array, but in discrete bits (8 bits = 1 byte)
        bytes_to_bytebits(raw_bytes, sizeof(raw_bytes), raw_bits); // Place byte array into bit array

        DbpString("[=] Deploying payload...");
        
        // parameters adopted from those in proxmark3/client/src/cmdlfsecurakey.c
        CmdASKsimTAG((uint8_t) 1, (uint8_t) 0, (uint8_t) 0, (uint8_t) 40, (uint16_t) sizeof(raw_bits), (uint8_t) raw_bits, true);

        DbpString("[=] Deploying payload finished!");
            
            
    }

    DbpString("[=] Shell shalln't be occupied -- Goodnight! 🌙");
    
    LEDsoff();
}
