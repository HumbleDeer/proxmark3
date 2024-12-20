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
//-----------------------------------------------------------------------------

#include "standalone.h" // standalone definitions
#include "proxmark3_arm.h"
#include "appmain.h"
#include "fpgaloader.h"
#include "lfops.h"
#include "util.h"
#include "dbprint.h"

// Declarations, not worth putting these in an extra header file.
static void bytes_to_bytebits(const void *src, const size_t srclen, void *dest);

#define raw_hex "7FCC0004436002AF34880000"
static uint8_t raw_bytes[12] = {0x7F, 0xCC, 0x00, 0x04, 0x43, 0x60, 0x02, 0xAF, 0x34, 0x88, 0x00, 0x00};

void ModInfo(void) {
    DbpString("Secura Key Radio Key (125 kHz) simulator (HumbleDeer)");
}

void RunMod(void) {    

    StandAloneMode();

    FpgaDownloadAndGo(FPGA_BITSTREAM_LF);

    Dbprintf(">>  LF Securakey Sim started  <<");

#define STATE_SIM 1

    for (;;) {

        WDT_HIT();

        // exit the main loop and wait for expected USB command.
        if (data_available()) break;

        // Was the button pressed for longer than X milliseconds?
        int button_pressed = BUTTON_HELD(300);
        if (button_pressed != BUTTON_HOLD){
            continue;
        }

        LED_C_ON();   // Simulation indicator
        LED_D_OFF();

        //WAIT_BUTTON_RELEASED();

        Dbprintf("Simulating with Radio Key (raw): "_YELLOW_("0x%s"), raw_hex);

        uint8_t raw_bits[sizeof(raw_bytes) * 8]; // Make array w/ size of raw[] array, but in discrete bits (8 bits = 1 byte)
        bytes_to_bytebits(raw_bytes, sizeof(raw_bytes), raw_bits); // Place byte array into bit array

        DbpString("[=] Simulator activated");
        
        // parameters adopted from those in proxmark3/client/src/cmdlfsecurakey.c
        CmdASKsimTAG((uint8_t) 1, (uint8_t) 0, (uint8_t) 0, (uint8_t) 40, (uint16_t) sizeof(raw_bits), raw_bits, true);

        DbpString("[=] Simulator deactivated");
            
    }

    DbpString("[=] Shell shalln't be occupied -- Goodnight! 🌙");
    
    LEDsoff();
}

static void bytes_to_bytebits(const void *src, const size_t srclen, void *dest) {
    uint8_t *s = (uint8_t *)src, *d = (uint8_t *)dest;
    size_t i = srclen * 8, j = srclen;

    while (j--) {
        uint8_t b = s[j];
        d[--i] = (b >> 0) & 1;
        d[--i] = (b >> 1) & 1;
        d[--i] = (b >> 2) & 1;
        d[--i] = (b >> 3) & 1;
        d[--i] = (b >> 4) & 1;
        d[--i] = (b >> 5) & 1;
        d[--i] = (b >> 6) & 1;
        d[--i] = (b >> 7) & 1;
    }
}  // 