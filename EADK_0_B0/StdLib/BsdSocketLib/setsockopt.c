/** @file
  Implement the setsockopt API.

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <SocketInternals.h>


/**
  Set the socket options

  @param [in] s               Socket file descriptor returned from ::socket.
  @param [in] level           Option protocol level
  @param [in] option_name     Name of the option
  @param [in] option_value    Buffer containing the option value
  @param [in] option_len      Length of the value in bytes

  @retval   Zero (0) upon success
  @retval   Minus one (-1) upon failure, errno set with additional error information

**/
int
setsockopt (
  IN int s,
  IN int level,
  IN int option_name,
  IN CONST void * option_value,
  IN socklen_t option_len
  )
{
  int OptionStatus;
  EFI_SOCKET_PROTOCOL * pSocketProtocol;
  EFI_STATUS Status;
  
  //
  //  Locate the context for this socket
  //
  pSocketProtocol = BslFdToSocketProtocol ( s, NULL, &errno );
  if ( NULL != pSocketProtocol ) {
    //
    //  Set the socket option
    //
    Status = pSocketProtocol->pfnOptionSet ( pSocketProtocol,
                                             level,
                                             option_name,
                                             option_value,
                                             option_len,
                                             &errno );
  }
  
  //
  //  Return the operation stauts
  //
  OptionStatus = ( 0 == errno ) ? 0 : -1;
  return OptionStatus;
}
