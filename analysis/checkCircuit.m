function checkCircuit(circuit)
    mnapoly_filename = strcat('../data/', circuit,'_mnapoly.tab');
    mnae_filename = strcat('../results/', circuit,'.tab');
    fprintf('Loading files...\n');
    mnapoly = importdata(mnapoly_filename);
    mnae = importdata(mnae_filename);
    fprintf('Files loaded...\n');
    if isstruct(mnae) && isstruct(mnapoly)
        for i=1:size(mnae.colheaders,2)
            mnae_index(i) = i;
            mnapoly_index(i) = find(strcmp(mnapoly.colheaders, mnae.colheaders{i}));
        end
        endRows = min(size(mnapoly.data,1),size(mnae.data,1));
        if endRows
            diff = mnae.data(1:endRows,mnae_index) - mnapoly.data(1:endRows,mnapoly_index);
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
