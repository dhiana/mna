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
    checkCircuit(circuit)
end
