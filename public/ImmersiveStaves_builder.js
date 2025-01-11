const fs = require("fs")
const types = [
    "Alteration",
    "Conjuration",
    "Restoration",
    "Destruction",
    "Illusion",
    "Falmer",
    "Forsworn",
]

const values = [
    {
        "Name":"Amber",
        "Description":"{Type} spells are 20% stronger. Consumes Stamina.",
        "Main":{
            "MagnitudePercentage": 120,
            "AreaPercentage": 100,
            "DurationPercentage": 120,
            "CostPercentage": 100,
            "ChargingTimePercentage": 100,
            "CostActorValue":"stamina"
        },
        "Other":{
            "MagnitudePercentage": 100,
            "AreaPercentage": 100,
            "DurationPercentage": 100,
            "CostPercentage": 100,
            "ChargingTimePercentage": 100,
            "CostActorValue":"stamina"
        }
    },
    {
        "Name":"Arcane",
        "Description":"{Type} spells are 20% stronger. Consumes Health.",
        "Main":{
            "MagnitudePercentage": 120,
            // "AreaPercentage": 100,
            "DurationPercentage": 120,
            "CostPercentage": 100,
            "ChargingTimePercentage": 100,
            "CostActorValue": "health",
        },
        "Other":{
            "MagnitudePercentage": 100,
            // "AreaPercentage": 100,
            "DurationPercentage": 100,
            "CostPercentage": 100,
            "ChargingTimePercentage": 100,
            "CostActorValue": "health",
        }
    },
    {
        "Name":"Inferno",
        "Description":"{Type} spells are 100% stronger, other spells are 50% stronger. All spells cost 100% more mana. Consumes Magicka.",
        "Main":{
            // "AreaPercentage": 100,
            "ChargingTimePercentage": 150,
            "MagnitudePercentage": 200,
            "DurationPercentage": 200,
            "CostPercentage": 200
        },
        "Other":{
            // "AreaPercentage": 100,
            "ChargingTimePercentage": 150,
            "MagnitudePercentage": 150,
            "DurationPercentage": 150,
            "CostPercentage": 200
        }
    },
    {
        "Name":"Ivory",
        "Description":"All spells cost 50% less mana. {Type} spells are 20% weaker. other spells are 50% weaker. Consumes Magicka.",
        "Main":{
            // "AreaPercentage": 100,
            "CostPercentage": 50,
            "MagnitudePercentage": 80,
            "DurationPercentage": 80,
            "ChargingTimePercentage": 80
        },
        "Other":{
            // "AreaPercentage": 100,
            "CostPercentage": 50,
            "MagnitudePercentage": 50,
            "DurationPercentage": 50,
            "ChargingTimePercentage": 80
        }
    },
    {
        "Name":"Mystic",
        "Description":"{Type} spells are 20% stronger. Consumes Charge.",
        "Main":{
            "MagnitudePercentage": 120,
            // "AreaPercentage": 100,
            "DurationPercentage": 120,
            "CostPercentage": 100,
            "ChargingTimePercentage": 80,
            "CostActorValue": "charge"
        },
        "Other":{
            "MagnitudePercentage": 100,
            // "AreaPercentage": 100,
            "DurationPercentage": 100,
            "CostPercentage": 100,
            "ChargingTimePercentage": 80,
            "CostActorValue": "charge"
        }
    }
]
const result = []
let descriptions = ""
for(const value of values){
    const {Main, Other, Name,Description} = value
    for(const type of types){
        const obj = {
            "Keyword":`IS_${Name}${type}`
        }
        if(type == "Falmer"){
            const current = {...Main}
            current["ChargingTimePercentage"] *= 2;
            obj["Default"] = current
        } else if (type == "Forsworn"){
            const current = {...Other}
            current["ChargingTimePercentage"] /= 2;
            obj["Default"] = current
        }
        else
        {
            obj[type] = Main
            obj["Default"] = Other
        }
        let desc = Description.replace(/\{Type\}/gi, type);
        desc = desc.replace(/(\d+|Stamina|Magicka|Charge|Health)/gi, "<font color=\"#FFFFFF\">$1</font>");
        descriptions += `IS_${Name}Staff${type}|${desc}\n`;
        result.push(obj)
    }
}
fs.writeFileSync("ImmersiveStaves_ISTV.json", JSON.stringify(result, null, 4))
fs.writeFileSync("ImmersiveStaves_DESC.ini", descriptions)