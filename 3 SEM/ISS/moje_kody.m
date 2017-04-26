% xgross09
% Grossmann Jan

%------1------
info = audioinfo('xgross09.wav');

%------2------
[s, Fs] = audioread('xgross09.wav');
X = fft(s);
Xshow = abs(X(1:8001));
k = 0:8000;
plot(k,Xshow);
xlabel('Hz');

%------3------
[mms, mmsi] = max(abs(X))

%------4------
A = [1.000 0.2289,   0.4662];
B = [0.2324,  -0.4112,   0.2324];
zplane(B,A);

p = roots(A);
if (isempty(p) | abs(p) < 1)
    disp('stabilni...')
else
    disp('NESTABILNI !!!')
end

%------5------
fq = freqz(B,A,8000);
plot(abs(fq));
xlabel('Hz');


%------6------
sf = filter(B,A,s);
XX = fft(sf);
XX = XX(1:8000);
plot(1:8000,abs(XX));
xlabel('Hz');

%------7------
[mmsf, mmsif] = max(abs(XX))


%------9------
Rv = xcorr(s,'Biased');
k = -50:50;
plot(k, Rv(Fs-1-50:Fs-1+50));
disp(Rv(Fs-1+10));

%------10------
%------Z grafu------

%------11------
en = s(1:length(s)-10);
enn = s(11:length(s));
xmin = min(min(s));
xmax = max(max(s));
x = linspace(xmin,xmax,100);
[h,p,r] = hist2opt(en, enn,x);
imagesc(-x,x,p);
axis xy;
colorbar;
xlabel('x2');
ylabel('x1');
