{ ////////////////////////////////////////////////////////////////////////// }
{ // Events                                                                  }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Serve para obter controlo do mouse e do keyboard num so package         }
{ //                                                                         }
unit Events;

interface

const
  { //////////////////////////////////////////////////////////////////////// }
  { // Numero maximo de events na lista                                      }
  { //                                                                       }
  MaxEventQueue = 511;

type
  { //////////////////////////////////////////////////////////////////////// }
  { // Um so event                                                           }
  { //                                                                       }
  TQueueEvent = array[0..15] of byte;

  { //////////////////////////////////////////////////////////////////////// }
  { // A lista de eventos                                                    }
  { //                                                                       }
  TQueue = array[0..MaxEventQueue] of TQueueEvent;

const
  EVN_None        = $00;
  EVN_KeyUp       = $03;
  EVN_KeyDown     = $05;
  EVN_CtrlBreak   = $07;

var
  { //////////////////////////////////////////////////////////////////////// }
  { // No caso de ficar muito tempo sem ser lida a lista pode dar a volta    }
  { // e comecar a escrever por cima dos dados velhos                        }
  { //                                                                       }
  EventsOverrun : boolean;

{ ////////////////////////////////////////////////////////////////////////// }
{ // GetNextEvent                                                            }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Preenche com uma copia do primeiro evento da lista o buffer passado ah  }
{ // funcao                                                                  }
{ //                                                                         }
function  GetNextEvent(var event : TQueueEvent) : boolean;

{ ////////////////////////////////////////////////////////////////////////// }
{ // ClearEventQueue                                                         }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Limpa a lista de eventos                                                }
{ //                                                                         }
procedure ClearEventQueue;

{ ////////////////////////////////////////////////////////////////////////// }
{ // InitEventHandling                                                       }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Inicializa tudo e mais alguma coisa de maneira a que se possa receber   }
{ // os eventos                                                              }
{ //                                                                         }
procedure InitEventHandling;

{ ////////////////////////////////////////////////////////////////////////// }
{ // ShutDownEventHandling                                                   }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Esta na hora de fechar a loja. Toca a por tudo como estava quando       }
{ // comecamos                                                               }
{ //                                                                         }
procedure ShutDownEventHandling;

{ ////////////////////////////////////////////////////////////////////////// }
{ // IsEventAvailable                                                        }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Retorna TRUE se estiver algum evento ah espera e se a libraria ja tiver }
{ // sido inicializada. Caso contrario retorna FALSE                         }
{ //                                                                         }
function  IsEventAvailable : boolean;

implementation

uses dos;

var
  { //////////////////////////////////////////////////////////////////////// }
  { // A lista onde se guardam os eventos                                    }
  { //                                                                       }
  Queue           : TQueue;

  { //////////////////////////////////////////////////////////////////////// }
  { // "Pointers" para dentro da lista de eventos                            }
  { //                                                                       }
  NextFreeEvent,
  CurrEvent       : integer;

  { //////////////////////////////////////////////////////////////////////// }
  { // Os antigos interrupt handlers                                         }
  { //                                                                       }
  OldKeybInt,
  OldCtrlBreakInt : pointer;

  { //////////////////////////////////////////////////////////////////////// }
  { // TRUE se a unit ja foi inicializada                                    }
  { //                                                                       }
  Inited,

  { //////////////////////////////////////////////////////////////////////// }
  { // TRUE se estivermos a aceitar eventos                                  }
  { //                                                                       }
  EventsActive    : boolean;

procedure DisableInts; inline($FA); { Esta funcao "desliga" os interrupts    }
procedure EnableInts;  inline($FB); { Esta funcao "liga" os interrupts       }

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{$F+,S-,W-}

{ ////////////////////////////////////////////////////////////////////////// }
{ // KeybInt (interrupt handler)                                             }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Este ISRH trata de ler a tecla do keyboard e gerar o evento             }
{ // correspondente.                                                         }
{ //                                                                         }
procedure KeybInt(Flags,CS,IP,AX,BX,CX,DX,SI,DI,DS,ES,BP : Word); interrupt;
var
  t : byte;

begin

  { //////////////////////////////////////////////////////////////////////// }
  { // Nao eh obrigatorio... mas pode evitar certas complicacoes             }
  { //                                                                       }
  DisableInts;

  { //////////////////////////////////////////////////////////////////////// }
  { // Leh a tecla do controlador do keyboard                                }
  { //                                                                       }
  t := port[$60];

  { //////////////////////////////////////////////////////////////////////// }
  { // So vamos processar esta tecla se estivermos a aceitar eventos         }
  { //                                                                       }
  if EventsActive then
  begin

    { ////////////////////////////////////////////////////////////////////// }
    { // Se o bit 7 do byte lido estiver a 0 entao esta tecla foi agora      }
    { // para baixo, caso contario veio para cima                            }
    { //                                                                     }
    Queue[NextFreeEvent][0] := EVN_KEYDOWN;

    if (t and $80) <> 0 then
    begin
      Queue[NextFreeEvent][0] := EVN_KEYUP;
    end;

    { ////////////////////////////////////////////////////////////////////// }
    { // Guarda a tecla que originou o evento. Convem remover o bit 7 da     }
    { // tecla visto que essa indicacao ja eh dado pelo tipo de evento       }
    { //                                                                     }
    Queue[NextFreeEvent][1] := (t and $7F);

    { ////////////////////////////////////////////////////////////////////// }
    { // Incrementa o index para o proximo evento                            }
    { //                                                                     }
    inc(NextFreeEvent);

    { ////////////////////////////////////////////////////////////////////// }
    { // Trata de fazer o wrap-around para evitar escrever fora do array!    }
    { //                                                                     }
    if nextfreeevent > MaxEventQueue then NextFreeEvent := 0;

    { ////////////////////////////////////////////////////////////////////// }
    { // Se depois de incrementar este index for igual ao index do evento    }
    { // que o programa vai ler a seguir entao temos uma situacao de overrun }
    { // que ainda nao aconteceu mas no caso de acontecer algum event antes  }
    { // do evento ser lido pelo programa entao estamos mal!!!               }
    { //                                                                     }
    if NextFreeEvent = CurrEvent then EventsOverrun := TRUE;
  end;

  { //////////////////////////////////////////////////////////////////////// }
  { // Este pedaco de codigo faz um tipo de reset qq ao controlador do       }
  { // keyboard. Eu ja nao me lembro porque que tenho ke fazer isto mas sei  }
  { // que a respostas esta nas Norton User Guides acerca do keyboard.       }
  { //                                                                       }
  { // NOTA : Nao tenho a certeza mas se este codigo nao estiver presente    }
  { //        a libraria funcionan na mesma. DAMN! Where did I put those     }
  { //        Norton Guides!!!                                               }
  t := port[$61];
  port[$61] := t or $80;
  port[$61] := t;

  { //////////////////////////////////////////////////////////////////////// }
  { // Os interrupts ja podem voltar                                         }
  { //                                                                       }
  EnableInts;

  { //////////////////////////////////////////////////////////////////////// }
  { // Sinalizar o fim de interrupt handler ao hardware (PIC)                }
  { //                                                                       }
  port[$20] := $20;
end;

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{ ////////////////////////////////////////////////////////////////////////// }
{ // CtrlBreakInt (interrupt handler)                                        }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Este ISRH trata de sinalizar o Ctrl+Break ao programa.                  }
{ //                                                                         }
procedure CtrlBreakInt(Flags,CS,IP,AX,BX,CX,DX,SI,DI,DS,ES,BP :
Word);interrupt;
begin

  { //////////////////////////////////////////////////////////////////////// }
  { // Eu nao tenho nada contra os interrupts mas nao quero que algum destes }
  { // interrupt handler aconteca a meio de um outro e estrague as variaveis }
  { //                                                                       }
  DisableInts;

  { //////////////////////////////////////////////////////////////////////// }
  { // So vamos notificar o Ctrl+Break se estivermos a aceitar eventos       }
  { //                                                                       }
  if EventsActive then
  begin

    { ////////////////////////////////////////////////////////////////////// }
    { // Ctrl+Break                                                          }
    { //                                                                     }
    Queue[NextFreeEvent][0] := EVN_CTRLBREAK;

    { ////////////////////////////////////////////////////////////////////// }
    { // Incrementa o index para o proximo evento                            }
    { //                                                                     }
    inc(NextFreeEvent);

    { ////////////////////////////////////////////////////////////////////// }
    { // Trata de fazer o wrap-around para evitar escrever fora do array!    }
    { //                                                                     }
    if nextfreeevent > MaxEventQueue then NextFreeEvent := 0;

    { ////////////////////////////////////////////////////////////////////// }
    { // Se depois de incrementar este index for igual ao index do evento    }
    { // que o programa vai ler a seguir entao temos uma situacao de overrun }
    { // que ainda nao aconteceu mas no caso de acontecer algum event antes  }
    { // do evento ser lido pelo programa entao estamos mal!!!               }
    { //                                                                     }
    if NextFreeEvent = CurrEvent then EventsOverrun := TRUE;

  end;

  { //////////////////////////////////////////////////////////////////////// }
  { // Os interrupts ja podem voltar                                         }
  { //                                                                       }
  EnableInts;

  { //////////////////////////////////////////////////////////////////////// }
  { // Sinalizar o fim de interrupt handler ao hardware (PIC)                }
  { //                                                                       }
  port[$20] := $20;

end;

{$F-,S+}

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{ ////////////////////////////////////////////////////////////////////////// }
{ // ClearEventQueue                                                         }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Limpa a lista de eventos                                                }
{ //                                                                         }
procedure ClearEventQueue;
begin

  { //////////////////////////////////////////////////////////////////////// }
  { // Nao faz sentido limpar uma lista de eventos que ainda nem esta a ser  }
  { // usada!!!                                                              }
  { //                                                                       }
  if not Inited then exit;

  { //////////////////////////////////////////////////////////////////////// }
  { // Quando isto acontece quer dizer que a lista ou esta vazia ou esta     }
  { // prestes a sofrer um overrun. Neste caso eh porque a lista fica vazia  }
  { //                                                                       }
  CurrEvent := NextFreeEvent;

  { //////////////////////////////////////////////////////////////////////// }
  { // Se a lista esta vazia nao pode estar a acontecer um overrun           }
  { //                                                                       }
  EventsOverRun   := FALSE;

end;

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{ ////////////////////////////////////////////////////////////////////////// }
{ // InitEventHandling                                                       }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Inicializa tudo e mais alguma coisa de maneira a que se possa receber   }
{ // os eventos                                                              }
{ //                                                                         }
procedure InitEventHandling;
begin

  { //////////////////////////////////////////////////////////////////////// }
  { // Inicializar duas vezes seguidas tem dois problemas                    }
  { //                                                                       }
  { //   1 - Eh estupido!                                                    }
  { //                                                                       }
  { //   2 - Nao dava jeito nenhum ter os handlers antigos iguais aos novos  }
  { //       (vejam o codigo para perceber)                                  }
  { //                                                                       }
  if Inited then exit;

  { //////////////////////////////////////////////////////////////////////// }
  { // Convem inicializar umas quantas variaveis para evitar confusoes       }
  { //                                                                       }

  { //////////////////////////////////////////////////////////////////////// }
  { // Nao convem que assim que se mudem os handlers este comecem a criar    }
  { // eventos                                                               }
  { //                                                                       }
  EventsActive  := FALSE;

  { //////////////////////////////////////////////////////////////////////// }
  { // Eh dificil fazer um overrun logo ao principio                         }
  { //                                                                       }
  EventsOverrun       := FALSE;

  { //////////////////////////////////////////////////////////////////////// }
  { // A lista de eventos encontra-se vazia                                  }
  { //                                                                       }
  CurrEvent     := 0;
  NextFreeEvent := CurrEvent;

  { //////////////////////////////////////////////////////////////////////// }
  { // Guardar os interrupt handler antigos                                  }
  { //                                                                       }
  GetIntVec($1B,OldCtrlBreakInt);
  GetIntVec($09,OldKeybInt);

  { //////////////////////////////////////////////////////////////////////// }
  { // Instalar os novos interrupt handlers                                  }
  { //                                                                       }
  SetIntVec($1B,Addr(CtrlBreakInt));
  SetIntVec($09,Addr(KeybInt));

  { //////////////////////////////////////////////////////////////////////// }
  { // Se chegamos aqui entao a inicializao correu bem e nesse caso podemos  }
  { // ligar os eventos                                                      }
  { //                                                                       }
  Inited       := TRUE;
  EventsActive := TRUE;

end;

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{ ////////////////////////////////////////////////////////////////////////// }
{ // ShutDownEventHandling                                                   }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Esta na hora de fechar a loja. Toca a por tudo como estava quando       }
{ // comecamos                                                               }
{ //                                                                         }
procedure ShutDownEventHandling;
begin

  { //////////////////////////////////////////////////////////////////////// }
  { // Desinicializar duas vezes seguidas tem um problema                    }
  { //                                                                       }
  { //   1 - Eh estupido!                                                    }
  { //                                                                       }
  if not Inited then exit;

  { //////////////////////////////////////////////////////////////////////// }
  { // Desligar os eventos                                                   }
  { //                                                                       }
  EventsActive := FALSE;

  { //////////////////////////////////////////////////////////////////////// }
  { // Por os handler antigos no sitio                                       }
  { //                                                                       }
  SetIntVec($1B,OldCtrlBreakInt);
  SetIntVec($09,OldKeybInt);

  { //////////////////////////////////////////////////////////////////////// }
  { // Limpar a lista de eventos                                             }
  { //                                                                       }
  CurrEvent     := 0;
  NextFreeEvent := CurrEvent;

  { //////////////////////////////////////////////////////////////////////// }
  { // Missao bem sucedida. Esta tudo como estava quando entramos            }
  { //                                                                       }
  Inited        := FALSE;

end;

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{ ////////////////////////////////////////////////////////////////////////// }
{ // GetNextEvent                                                            }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Preenche com uma copia do primeiro evento da lista o buffer passado ah  }
{ // funcao                                                                  }
{ //                                                                         }
function GetNextEvent(var event : TQueueEvent) : boolean;
begin

  { //////////////////////////////////////////////////////////////////////// }
  { // Convem presumir que nao vamos conseguir                               }
  { //                                                                       }
  GetNextEvent := FALSE;

  { //////////////////////////////////////////////////////////////////////// }
  { // Se a lista estiver vazia ou a libraria ainda nao tiver sido           }
  { // inicializada entao sai                                                }
  { //                                                                       }
  if (not inited) or (CurrEvent=NextFreeEvent) then exit;

  { //////////////////////////////////////////////////////////////////////// }
  { // Copia o evento                                                        }
  { //                                                                       }
  Event := Queue[CurrEvent];

  { //////////////////////////////////////////////////////////////////////// }
  { // Incrementa o index para o proximo evento                              }
  { //                                                                       }
  inc(CurrEvent);

  { //////////////////////////////////////////////////////////////////////// }
  { // Trata de fazer o wrap-around para evitar ler fora do array!           }
  { //                                                                       }
  if CurrEvent > MaxEventQueue then CurrEvent := 0;

  { //////////////////////////////////////////////////////////////////////// }
  { // Sucesso!                                                              }
  { //                                                                       }
  GetNextEvent := TRUE;

end;

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{ ////////////////////////////////////////////////////////////////////////// }
{ // IsEventAvailable                                                        }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Retorna TRUE se estiver algum evento ah espera e se a libraria ja tiver }
{ // sido inicializada. Caso contrario retorna FALSE                         }
{ //                                                                         }
function  IsEventAvailable : boolean;
begin

  { //////////////////////////////////////////////////////////////////////// }
  { // Quem nao perceber esta linha veja este bocado de codigo :             }
  { //                                                                       }
  { // IsEventAvailable := FALSE;                                            }
  { // if not Inited then exit;                                              }
  { // if (CurrEvent = NextFreeEvent) then exit;                             }
  { // IsEventAvailable := TRUE;                                             }
  { //                                                                       }
  { // Esta linha em baixo faz isto tudo. Compressao 4:1. Eat your heart     }
  { // out DriveSpace!!!                                                     }
  { //                                                                       }
  IsEventAvailable := ((not (CurrEvent=NextFreeEvent)) and Inited);

end;

{+--------------------------------------------------------------------------+}
{|                                                                          |}
{+--------------------------------------------------------------------------+}

{ ////////////////////////////////////////////////////////////////////////// }
{ // Main                                                                    }
{ ////////////////////////////////////////////////////////////////////////// }
{ // Por uma questao de boas maneiras inicializa as variaveis para valores   }
{ // seguros                                                                 }
{ //                                                                         }
begin
  NextFreeEvent := 0;
  CurrEvent     := 0;
  Inited        := FALSE;
  EventsActive  := FALSE;
  EventsOverrun := FALSE;
end.
