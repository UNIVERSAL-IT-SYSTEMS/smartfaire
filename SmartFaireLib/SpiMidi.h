// Copyright(c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the BSD 2 - Clause License.
// See License.txt in the project root for license information.

#include "stdafx.h"

void MusicShieldInit(bool realTimeMidiMode);
void noteOn(BYTE channel, BYTE note, BYTE attack_velocity);
void noteOff(BYTE channel, BYTE note, BYTE release_velocity);
