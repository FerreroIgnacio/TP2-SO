#!/usr/bin/env node

const fs = require('fs');

function generateSineTable() {
    const tableSize = 360;
    const multiplier = 1000;
    const values = [];
    

    // Generate sine values for 0-359 degrees
    for (let deg = 0; deg < tableSize; deg++) {
        const radians = (deg * Math.PI) / 180;
        const sineValue = Math.sin(radians);
        const scaledValue = Math.round(sineValue * multiplier);
        values.push(scaledValue);
    }
    
    // Generate C code
    let cCode = `// Precalculated sine table * ${multiplier} to avoid math.h\n`;
    cCode += `// 0° points right, angles increase counterclockwise\n`;
    cCode += `static const int64_t sinVec[${tableSize}] = {\n`;
    
    // Format values in rows of 10
    for (let i = 0; i < values.length; i++) {
        if (i % 10 === 0) {
            cCode += '    ';
        }
        
        cCode += values[i].toString().padStart(4, ' ');
        
        if (i < values.length - 1) {
            cCode += ',';
        }
        
        if (i % 10 === 9 || i === values.length - 1) {
            cCode += '\n';
        } else {
            cCode += ' ';
        }
    }
    
    cCode += '};\n\n';
    
    // Add helper functions
    cCode += `// Returns sine * ${multiplier}, no floating point needed\n`;
    cCode += `static int64_t sin(int64_t deg) {\n`;
    cCode += `    while (deg < 0) deg += 360;\n`;
    cCode += `    while (deg >= 360) deg -= 360;\n`;
    cCode += `    return sinVec[deg];\n`;
    cCode += `}\n\n`;
    
    cCode += `static int64_t cos(int64_t deg) {\n`;
    cCode += `    deg += 90;\n`;
    cCode += `    while (deg < 0) deg += 360;\n`;
    cCode += `    while (deg >= 360) deg -= 360;\n`;
    cCode += `    return sinVec[deg];\n`;
    cCode += `}\n`;
    
    return cCode;
}

function printTestValues() {
    console.log('\nTest values:');
    const testAngles = [0, 30, 45, 60, 90, 180, 270];
    
    for (const angle of testAngles) {
        const radians = (angle * Math.PI) / 180;
        const sin = Math.round(Math.sin(radians) * 1000);
        const cos = Math.round(Math.cos(radians) * 1000);
        console.log(`sin(${angle}°) = ${sin}, cos(${angle}°) = ${cos}`);
    }
}

// Main execution
const cCode = generateSineTable();

// Write to file
const filename = 'sin_table.c';
fs.writeFileSync(filename, cCode);
console.log(`\nSine table written to ${filename}`);

// Print test values
printTestValues();

// Print the generated code to console
console.log('\n--- Generated C Code ---');
console.log(cCode);