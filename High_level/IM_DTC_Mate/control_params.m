%sample time
Tsamp = 0.0001;

%reference values
Psiref = 1;
Tref = 12;
Tref_switch = 1;

%Controller parameters
%->PID
Kp = 10;
Kd = 3;
Ki = 7;
min_PID = 0;
max_PID = 10;
%->Hist
Psierr = 0.02;
Trqerr = 1;

%Motor parameters
PolePairs = 2;
R1 = 1.7;
R2 = 1.41;
Lm = 92.6e-3;
Lsl = 5.4e-3;
Lrl = 4.9e-3;
L1 = Lsl + Lm;
L2 = Lrl + Lm;
Tr = L2/R2;
Ts = L1/R1;

%Inverter paramteters
Udc = 60;