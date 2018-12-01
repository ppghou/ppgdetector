L_max = length(time); %Total length
Fs = 50; %Sampling Frequency
span = 2; %Length of Signal
second = 2; %The second which is being measured
Wc = 2*5/Fs; %Cut off frequency
[b,a]=butter(4,Wc); % Butterwroth filter design
Signal = G;
Signal = Signal-Signal(1);
Signal_Filtered = filter(b,a,Signal);
plot(time,Signal)
hold on
plot(time,Signal_Filtered)
hold off
filter_peaks = [];
filter_peak_num = 0;
for i =2:L_max-1
    temp_peak = 0;
    if(Signal_Filtered(i-1)<Signal_Filtered(i) && Signal_Filtered(i+1)<Signal_Filtered(i))
        temp_peak = i;
        filter_peaks(filter_peak_num+1) = i;
        filter_peak_num = filter_peak_num + 1;
    end
end
peaks = zeros(length(filter_peak_num)-1);
amplitude = zeros(length(filter_peak_num)-1);
for i =1:filter_peak_num-1
    left_edge = filter_peaks(i);
    right_edge = filter_peaks(i+1);
    [amp,pos] = max(G(left_edge:right_edge,1:1));
    peaks(i) = left_edge + pos - 1;
    amplitude(i) = G(peaks(i));
end
peak_time_pos = zeros(length(filter_peak_num)-1);
for i = 1:filter_peak_num-1
    peak_time_pos(i) = time(peaks(i));
end

plot(time,Signal+G(1))
hold on
plot(time,Signal_Filtered+G(1))
plot(peak_time_pos,amplitude)
hold off

heart_rate = zeros(L_max,1);
for i = 1:length(peaks)-1
    left_edge = peaks(i);
    right_edge = peaks(i+1);
    hr = 60/(peak_time_pos(i+1)-peak_time_pos(i));
    heart_rate(left_edge:right_edge,1:1) = hr;
end

plot(time,heart_rate)
hold on

second = 1:1:fix(L_max/Fs);
heart_rate_second = zeros(length(second),1);
for i=1:length(second)
    heart_rate_second(i) = heart_rate(i*Fs);
end

plot(second,heart_rate_second);


% while(Fs*second+span*Fs<=L_max)
%     t_interval = time(Fs*second-span*Fs+1:Fs*second+span*Fs,1:1);% The 10s interval which will be calculated
%     Signal = G(Fs*second-span*Fs+1:Fs*second+span*Fs,1:1);
%     Signal = Signal-Signal(1);
%     Signal_Filtered = filter(b,a,Signal);
%     %plot(t_interval,Signal)
%     %hold on;
%     %plot(t_interval,Signal_Filtered)
%     peaks = [];
%     peak_num = 0;
%     for i =2:span*2*Fs-1
%         temp_peak = 0;
%         if(Signal_Filtered(i-1)<Signal_Filtered(i) && Signal_Filtered(i+1)<Signal_Filtered(i))
%             temp_peak = i;
%             peaks(peak_num+1) = i;
%             peak_num = peak_num + 1;
%         end
%     end
%     left_peak = Signal(peaks(1):peaks(2),1:1);
%     right_peak = Signal(peaks(peak_num-1):peaks(peak_num),1:1);
%     [left_max,left_peak_pos] = max(left_peak);
%     [right_max,right_peak_pos] = max(right_peak);
%     left_peak_pos = peaks(1) + left_peak_pos - 1;
%     right_peak_pos = peaks(peak_num-1) + right_peak_pos - 1;
%     period = (t_interval(right_peak_pos) - t_interval(left_peak_pos))/(peak_num - 2);
%     heart_rate(index) = 60/period;
%     time_seconds(index) = second;
%     index = index+1;
%     second = second+1;
% end