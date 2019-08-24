//Modulo de Filtros
//Los Filtros son pasa banda a fin de acotar el espectro a lo requerido

//Filtro Pasa Alto
/*
digital finite impulse response (FIR) filter designed with the Android FIR Filter Designer application from www.ledin.com.
Filter specifications:
Filter type          : Bandpass
Sample Rate          : 44100 Hz
Passband gain        : 2 dB
Stopband gain        : -40 dB
Low band edge        : 750 Hz
High band edge       : 9000 Hz
Mid band lower edge  : 1200 Hz
Mid band upper edge  : 8000 Hz

Filter design results:
Filter order         : 140
Passband weight      : 0,155621
*/
#define N 141
static const double h[N] = 
{
0.01032684474384226,
    0.012225172889283637,
    0.008760010997854435,
    -1.884617624506811E-4,
    -0.006769869901274298,
    -0.005196169519879638,
    0.0013883631241722023,
    0.004186967040043251,
    -8.088571480304196E-4,
    -0.007700308303938531,
    -0.007844341449046865,
    -9.695078809362413E-4,
    0.0038653053800295975,
    -3.5777931630084014E-4,
    -0.009118658654006333,
    -0.011398901377668686,
    -0.0038142498639614146,
    0.004206954256192015,
    0.0019581244012400363,
    -0.008370908772183093,
    -0.013518680824449155,
    -0.0059397289677794425,
    0.005691725121428115,
    0.006754093975741145,
    -0.004302197150252917,
    -0.012918817571828363,
    -0.006611722608892115,
    0.008272567145354579,
    0.013635185220664283,
    0.0028514985519416795,
    -0.009673249685238862,
    -0.006253605785003109,
    0.010772333874336768,
    0.02083796805074204,
    0.011286333142552176,
    -0.005358277268913912,
    -0.006523758291039455,
    0.010997597755413627,
    0.025618366069706106,
    0.018171438712754364,
    -0.002485079439500246,
    -0.009671354972118644,
    0.006591244391589482,
    0.02531982005692116,
    0.02077610086642148,
    -0.0034499106821601544,
    -0.017558455530498642,
    -0.0038985779015801056,
    0.018651962415680658,
    0.01784297663579459,
    -0.009366811676953541,
    -0.03080176674170918,
    -0.02019801929576428,
    0.006785692346148284,
    0.01078556203443162,
    -0.01934342232375387,
    -0.04878524206002147,
    -0.04042992824261526,
    -0.00599618552644363,
    0.004863690870700601,
    -0.030535049795598446,
    -0.07203834439110841,
    -0.06361534181641862,
    -0.010687748295740096,
    0.014438034876384667,
    -0.039338012306276475,
    -0.12060662875324989,
    -0.1105169133662793,
    0.04635489692871118,
    0.2585296798993385,
    0.3573319826663677,
    0.2585296798993385,
    0.04635489692871118,
    -0.1105169133662793,
    -0.12060662875324989,
    -0.039338012306276475,
    0.014438034876384667,
    -0.010687748295740096,
    -0.06361534181641862,
    -0.07203834439110841,
    -0.030535049795598446,
    0.004863690870700601,
    -0.00599618552644363,
    -0.04042992824261526,
    -0.04878524206002147,
    -0.01934342232375387,
    0.01078556203443162,
    0.006785692346148284,
    -0.02019801929576428,
    -0.03080176674170918,
    -0.009366811676953541,
    0.01784297663579459,
    0.018651962415680658,
    -0.0038985779015801056,
    -0.017558455530498642,
    -0.0034499106821601544,
    0.02077610086642148,
    0.02531982005692116,
    0.006591244391589482,
    -0.009671354972118644,
    -0.002485079439500246,
    0.018171438712754364,
    0.025618366069706106,
    0.010997597755413627,
    -0.006523758291039455,
    -0.005358277268913912,
    0.011286333142552176,
    0.02083796805074204,
    0.010772333874336768,
    -0.006253605785003109,
    -0.009673249685238862,
    0.0028514985519416795,
    0.013635185220664283,
    0.008272567145354579,
    -0.006611722608892115,
    -0.012918817571828363,
    -0.004302197150252917,
    0.006754093975741145,
    0.005691725121428115,
    -0.0059397289677794425,
    -0.013518680824449155,
    -0.008370908772183093,
    0.0019581244012400363,
    0.004206954256192015,
    -0.0038142498639614146,
    -0.011398901377668686,
    -0.009118658654006333,
    -3.5777931630084014E-4,
    0.0038653053800295975,
    -9.695078809362413E-4,
    -0.007844341449046865,
    -0.007700308303938531,
    -8.088571480304196E-4,
    0.004186967040043251,
    0.0013883631241722023,
    -0.005196169519879638,
    -0.006769869901274298,
    -1.884617624506811E-4,
    0.008760010997854435,
    0.012225172889283637,
    0.01032684474384226,
};

float filter_fpa(const double x_in)
{
    static int n = 0;
    static double x[N];
    int i;
    double y = 0.0;

    // Store the current input, overwriting the oldest input
    x[n] = x_in;

    // Multiply the filter coefficients by the previous inputs and sum
    for (i=0; i<N; i++)
    {
        y += h[i] * x[((N - i) + n) % N];
    }

    // Increment the input buffer index to the next location
    n = (n + 1) % N;

    return y;
}

//Fin Filtro Pasa Alto

//Filtro Pasa Bajo
/* 
digital finite impulse response (FIR) filter designed with the Android FIR Filter Designer application from www.ledin.com.
Filter specifications:
Filter type          : Bandpass
Sample Rate          : 44100 Hz
Passband gain        : 2 dB
Stopband gain        : -30 dB
Low band edge        : 10 Hz
High band edge       : 1600 Hz
Mid band lower edge  : 200 Hz
Mid band upper edge  : 1200 Hz

Filter design results:
Filter order         : 163
Passband weight      : 0,045725
*/

#define M 164
static const double g[M] = 
{
 -0.007020524768008338,
    -0.00236605962020389,
    -0.0027648958809981666,
    -0.0032022984896572263,
    -0.0036813430542951696,
    -0.004196008709096494,
    -0.004749767905979326,
    -0.005336706493180757,
    -0.005958816280463029,
    -0.006601237895589595,
    -0.007257696910113646,
    -0.007910587841387253,
    -0.008571003002405788,
    -0.00920506902675619,
    -0.009796875611449917,
    -0.010351232136151751,
    -0.010832946377702102,
    -0.011241417569977687,
    -0.01155209271702681,
    -0.011757995972796115,
    -0.011843017864234692,
    -0.011802425169035744,
    -0.011623263662520807,
    -0.011303342186094105,
    -0.010840542355128647,
    -0.010242836753490948,
    -0.009506988096557185,
    -0.008653434826638332,
    -0.0076913014051213325,
    -0.006642851393344013,
    -0.005530229837155691,
    -0.00437964476771148,
    -0.0032202817430321807,
    -0.0020850348843240232,
    -0.001006376368226376,
    -1.7331998842271803E-5,
    8.492707696676293E-4,
    0.0015596549955254318,
    0.002086719942384408,
    0.0024048392581252085,
    0.0024915933403090898,
    0.0023341350865936955,
    0.001920902651718919,
    0.0012523801920623831,
    3.338040977804878E-4,
    -8.200510444329546E-4,
    -0.002187956476344381,
    -0.0037381802340496844,
    -0.00543278503003174,
    -0.007226335393790388,
    -0.009068069945912666,
    -0.01089974271478068,
    -0.012662587444816973,
    -0.014292274079446637,
    -0.015726444773092745,
    -0.016902900074266182,
    -0.01776175896189267,
    -0.018248692048782066,
    -0.01831570800481619,
    -0.01792307121447935,
    -0.017039817913726527,
    -0.0156467312332678,
    -0.013736007944489861,
    -0.011312625267013111,
    -0.008394108705594835,
    -0.005011693554359321,
    -0.001207739746544875,
    0.002960909018778986,
    0.007429101156833737,
    0.012120245888475975,
    0.01695115946241483,
    0.021831160862019337,
    0.0266672825378666,
    0.031363490204489614,
    0.035825863274930814,
    0.03996222486418648,
    0.04368691684958511,
    0.046920899862925546,
    0.04959615593760716,
    0.05165471750346429,
    0.05305235265103823,
    0.0537588092388379,
    0.0537588092388379,
    0.05305235265103823,
    0.05165471750346429,
    0.04959615593760716,
    0.046920899862925546,
    0.04368691684958511,
    0.03996222486418648,
    0.035825863274930814,
    0.031363490204489614,
    0.0266672825378666,
    0.021831160862019337,
    0.01695115946241483,
    0.012120245888475975,
    0.007429101156833737,
    0.002960909018778986,
    -0.001207739746544875,
    -0.005011693554359321,
    -0.008394108705594835,
    -0.011312625267013111,
    -0.013736007944489861,
    -0.0156467312332678,
    -0.017039817913726527,
    -0.01792307121447935,
    -0.01831570800481619,
    -0.018248692048782066,
    -0.01776175896189267,
    -0.016902900074266182,
    -0.015726444773092745,
    -0.014292274079446637,
    -0.012662587444816973,
    -0.01089974271478068,
    -0.009068069945912666,
    -0.007226335393790388,
    -0.00543278503003174,
    -0.0037381802340496844,
    -0.002187956476344381,
    -8.200510444329546E-4,
    3.338040977804878E-4,
    0.0012523801920623831,
    0.001920902651718919,
    0.0023341350865936955,
    0.0024915933403090898,
    0.0024048392581252085,
    0.002086719942384408,
    0.0015596549955254318,
    8.492707696676293E-4,
    -1.7331998842271803E-5,
    -0.001006376368226376,
    -0.0020850348843240232,
    -0.0032202817430321807,
    -0.00437964476771148,
    -0.005530229837155691,
    -0.006642851393344013,
    -0.0076913014051213325,
    -0.008653434826638332,
    -0.009506988096557185,
    -0.010242836753490948,
    -0.010840542355128647,
    -0.011303342186094105,
    -0.011623263662520807,
    -0.011802425169035744,
    -0.011843017864234692,
    -0.011757995972796115,
    -0.01155209271702681,
    -0.011241417569977687,
    -0.010832946377702102,
    -0.010351232136151751,
    -0.009796875611449917,
    -0.00920506902675619,
    -0.008571003002405788,
    -0.007910587841387253,
    -0.007257696910113646,
    -0.006601237895589595,
    -0.005958816280463029,
    -0.005336706493180757,
    -0.004749767905979326,
    -0.004196008709096494,
    -0.0036813430542951696,
    -0.0032022984896572263,
    -0.0027648958809981666,
    -0.00236605962020389,
    -0.007020524768008338,
};

float filter_fpb(const double x_in)
{
    static int n = 0;
    static double x[M];
    int i;
    double y = 0.0;

    // Store the current input, overwriting the oldest input
    x[n] = x_in;

    // Multiply the filter coefficients by the previous inputs and sum
    for (i=0; i<M; i++)
    {
        y += g[i] * x[((M - i) + n) % M];
    }

    // Increment the input buffer index to the next location
    n = (n + 1) % M;

    return y;
}
