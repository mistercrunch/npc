{ ////////////////////////////////////////////////////////////////////////// }
{ // Gfx                                                                     }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Libraria grafica para o modo 13h                                        }
{ //                                                                         }
unit Gfx;

interface

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxGoGfxMode                                                            }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Inicializa o modo grafico                                               }
{ //                                                                         }
procedure GfxGoGfxMode;

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxGoTxtMode                                                            }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Retorna ao modo texto                                                   }
{ //                                                                         }
procedure GfxGoTxtMode;

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxPutPixel                                                             }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Poe um pixel no ecran                                                   }
{ //                                                                         }
procedure GfxPutPixel(x,y : integer; color : byte);

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxGetPixel                                                             }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Retorma a cor de um pixel no ecran                                      }
{ //                                                                         }
function  GfxGetPixel(x,y : integer) : byte;

implementation

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxGoGfxMode                                                            }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Inicializa o modo grafico                                               }
{ //                                                                         }
procedure GfxGoGfxMode; assembler;
asm
  mov  ax, 0013h { AH = 00h - Funcao SetMode | AL = 13h - Modo video desejado}
  int  10h       { 10h - Servicos de video do BIOS                           }
end;

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxGoTxtMode                                                            }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Retorna ao modo texto                                                   }
{ //                                                                         }
procedure GfxGoTxtMode; assembler;
asm
  mov  ax, 0003h { AH = 00h - Funcao SetMode | AL = 03h - Modo video desejado}
  int  10h       { 10h - Servicos de video do BIOS                           }
end;

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxPutPixel                                                             }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Poe um pixel no ecran                                                   }
{ //                                                                         }
procedure GfxPutPixel(x,y : integer; color : byte); assembler;
asm
  mov  ax, [y]        { AX = Y                                               }
  mov  bx, [x]        { BX = X                                               }
  add  bh, al         { BX = X+Y*256                                         }
  shl  ax, 6          { AX = Y*64                                            }
  add  ax, bx         { AX = X+Y*320                                         }
  mov  di, ax         { DI = AX                                              }
  mov  ax, 0A000h     {                                                      }
  mov  es, ax         { ES:DI = Endereco do Pixel                            }
  mov  al, [color]
  mov  es:[di], al    { ES:DI = Color - One Painted Pixel Comin' Up!         }
end;

{ ////////////////////////////////////////////////////////////////////////// }
{ // GfxGetPixel                                                             }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Retorma a cor de um pixel no ecran                                      }
{ //                                                                         }
function  GfxGetPixel(x,y : integer) : byte; assembler;
asm
  mov  ax, [y]        { AX = Y                                               }
  mov  bx, [x]        { BX = X                                               }
  add  bh, al         { BX = X+Y*256                                         }
  shl  ax, 6          { AX = Y*64                                            }
  add  ax, bx         { AX = X+Y*320                                         }
  mov  di, ax         { DI = AX                                              }
  mov  ax, 0A000h     {                                                      }
  mov  es, ax         { ES:DI = Endereco do Pixel                            }
  mov  al, es:[di]    { AL = Cor do Pixel                                    }
end;

end.