/** @file
  Copyright (c) 2016, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __NON_DISCOVERABLE_DEVICE_REGISTRATION_LIB_H__
#define __NON_DISCOVERABLE_DEVICE_REGISTRATION_LIB_H__

#include <Protocol/NonDiscoverableDevice.h>

/**
  Register a non-discoverable MMIO device

  @param[in]      DeviceType          The type of non-discoverable device
  @param[in]      DmaType             Whether the device is DMA coherent
  @param[in]      InitFunc            Initialization routine to be invoked when
                                      the device is enabled
  @param[in,out]  Handle              The handle onto which to install the
                                      non-discoverable device protocol.
                                      If Handle is NULL or *Handle is NULL, a
                                      new handle will be allocated.
  @param[in]      NumMmioResources    The number of UINTN base/size pairs that
                                      follow, each describing an MMIO region
                                      owned by the device

  @retval EFI_SUCCESS                 The registration succeeded.
  @retval Other                       The registration failed.

**/
EFI_STATUS
EFIAPI
RegisterNonDiscoverableMmioDevice (
  IN      NON_DISCOVERABLE_DEVICE_TYPE      Type,
  IN      NON_DISCOVERABLE_DEVICE_DMA_TYPE  DmaType,
  IN      NON_DISCOVERABLE_DEVICE_INIT      InitFunc,
  IN OUT  EFI_HANDLE                        *Handle OPTIONAL,
  IN      UINTN                             NumMmioResources,
  ...
  );

#endif
