%% Import data from text file.
% Script for importing data from the following text file:
%
%    C:\Users\Ludger Heide\Downloads\attitude.csv
%
% To extend the code to different selected data or a different text file,
% generate a function instead of a script.

% Auto-generated by MATLAB on 2016/04/12 17:54:20

%% Initialize variables.
filename = 'C:\Users\Ludger Heide\Downloads\attitude.csv';
delimiter = ',';

%% Format string for each line of text:
%   column1: double (%f)
%	column2: double (%f)
%   column3: double (%f)
%	column4: double (%f)
% For more information, see the TEXTSCAN documentation.
formatSpec = '%f%f%f%f%[^\n\r]';

%% Open the text file.
fileID = fopen(filename,'r');

%% Read columns of data according to format string.
% This call is based on the structure of the file used to generate this
% code. If an error occurs for a different file, try regenerating the code
% from the Import Tool.
dataArray = textscan(fileID, formatSpec, 'Delimiter', delimiter, 'EmptyValue' ,NaN, 'ReturnOnError', false);

%% Close the text file.
fclose(fileID);

%% Post processing for unimportable data.
% No unimportable data rules were applied during the import, so no post
% processing code is included. To generate code which works for
% unimportable data, select unimportable cells in a file and regenerate the
% script.

%% Create output variable
attitude = table(dataArray{1:end-1}, 'VariableNames', {'timestamp','course','pitch','roll'});

%% Clear temporary variables
clearvars filename delimiter formatSpec fileID dataArray ans;