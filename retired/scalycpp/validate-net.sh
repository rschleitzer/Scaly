#!/bin/sh
NSGMLS="dotnet run --project /Users/r.schleitzer/repos/openjade-net/src/Nsgmls/Nsgmls.csproj --"
$NSGMLS scaly.sgm > scaly-net.esis
diff scaly.esis scaly-net.esis
