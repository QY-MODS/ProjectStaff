const fs = require('fs');
const path = require('path');

function GetAllDirectories(...directoryPath){
    const result = []

    try {
      const entries = fs.readdirSync(path.join(...directoryPath), { withFileTypes: true });
    
      for(const entry of entries){
        result.push(entry.name)
      }
    } catch (err) {
      console.error(`Error reading directory: ${err.message}`);
    }

    return result
}

function readAllLines(...directoryPath) {
    try {
        const filePath = path.join(...directoryPath)
        // Ensure the file exists before attempting to read it
        if (!fs.existsSync(filePath)) {
            throw new Error(`File not found: ${filePath}`);
        }

        // Read the entire file content synchronously
        const data = fs.readFileSync(path.resolve(filePath), 'utf-8');

        // Split the content by lines (using \r\n for Windows, \n for Unix)
        const lines = data.split(/\r?\n/);

        return lines;
    } catch (err) {
        console.error(`Error reading file: ${err.message}`);
        throw err;  // Optionally rethrow or handle as needed
    }
}

function writeTextFile(text, ...directoryPath) {
    try {
        // Use resolve to ensure we're working with absolute paths and handle relative ones properly.
        const filePath = path.join(...directoryPath)


        // Write the text content synchronously to the file
        fs.writeFileSync(filePath, text, 'utf-8');

    } catch (err) {
        console.error(`Error writing file: ${err.message}`);
        throw err;  // Optionally rethrow or handle as needed
    }
}
const leveledListToAdd = GetAllDirectories("./data")

const chances = {
    "NPC_": 100,
    "CONT": 100,
}


let result = "[General]\n"

for(const leveledItemToAdd of leveledListToAdd){

    const leveledListToAddTo = GetAllDirectories("./data", leveledItemToAdd)
    result += `\n; ${leveledItemToAdd}\n`
    for(const leveledItemToAddTo of leveledListToAddTo){
        const containers = readAllLines("./data", leveledItemToAdd, leveledItemToAddTo)
        
        for(const container of containers){
            const [formType, editorId] = container.split(' ')
            result += `${editorId} = ${leveledItemToAdd}:1?${chances[formType]}\n`
        }
    }
}

writeTextFile(result, "../../ImmersiveStaves_CID.ini")