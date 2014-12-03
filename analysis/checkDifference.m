circuitsArray = [
    'bennetmult3';
    'chuapole   ';
    'chuapolf   ';
    'chuapolg   ';
    'chuapolh   ';
    'chuapolr   ';
    'diodop1    ';
    'diodop2    ';
    'ebersmoll  ';
    'mult       ';
    'prova      ';
    'teslak     ';
    'tracpol    ';
    'tractran   ';
    'tunel3     '
];


circuits = cellstr(circuitsArray);

for i=1:15
    circuit = circuits{i}
    mnapoly_filename = strcat('../data/', circuits{i},'_mnapoly.tab');
    mnae_filename = strcat('../results/', circuits{i},'.tab');
    fprintf('Loading files...\n');
    mnapoly = importdata(mnapoly_filename);
    mnae = importdata(mnae_filename);
    fprintf('Files loaded...\n');
    if isstruct(mnae) && isstruct(mnapoly)
        endRows = min(size(mnapoly.data,1),size(mnae.data,1));
        if endRows
            diff = mnae.data(1:endRows,:) - mnapoly.data(1:endRows,:);
            f = figure;
            subplot(1,2,1);
            plot(diff');
            title(circuit);
            xlabel('Variable');
            ylabel('Difference w.r.t MNAE');
            subplot(1,2,2);
            plot(diff);
            title(circuit);
            xlabel('Time');
            ylabel('Difference w.r.t MNAE');
            saveas(f, strcat(circuit,'.png'), 'png');
        else
            fprintf('There wasnt a full output...\n');
        end
    end
end
