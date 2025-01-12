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
        "Description":"Channeling Stamina thought this staff: Non {Type} cost 20% more to cast",
        "ForswornDescription":"Channeling Stamina thought this staff: Spells cost 20% less to cast",
        "FalmerDescription":"Channeling Stamina thought this staff: Spells are 20% stronger but cost 20% more to cast",
        "Main":{
            // "AreaPercentage": 100,
            // "MagnitudePercentage": 100,
            // "DurationPercentage": 100,
            // "CostPercentage": 100,
            // "ChargingTimePercentage": 100,
            "CostActorValue":"stamina"
        },
        "Other":{
            // "MagnitudePercentage": 100,
            // "AreaPercentage": 100,
            // "DurationPercentage": 100,
            // "ChargingTimePercentage": 100,
            "CostPercentage": 120,
            "CostActorValue":"stamina"
        }
    },
    {
        "Name":"Arcane",
        "Description":"Channeling Health thought this staff: Spells are 30% stronger, Non {Type} cost 20% more to cast",
        "ForswornDescription":"Channeling Health thought this staff: Spells are 30% stronger, Spells cost 20% less to cast",
        "FalmerDescription":"Channeling Health thought this staff: Spells are 50% stronger but cost 50% more to cast",
        "Main":{
            // "AreaPercentage": 100,
            // "ChargingTimePercentage": 100,
            "MagnitudePercentage": 130,
            "DurationPercentage": 130,
            "CostPercentage": 130
        },
        "Other":{
            // "AreaPercentage": 100,
            // "ChargingTimePercentage": 100,
            "MagnitudePercentage": 130,
            "DurationPercentage": 130,
            "CostPercentage": 150
        }
    },
    {
        "Name":"Inferno",
        "Description":"Channeling Magicka thought this staff: Spells are 30% stronger, {Type} spells cost 30% more to cast, other spells cost 50% more to cast",
        "ForswornDescription":"Channeling Magicka thought this staff: Spells are 30% stronger, Spells cost 10% more to cast",
        "FalmerDescription":"Channeling Magicka thought this staff: Spells are 50% stronger but cost 50% more to cast",
        "Main":{
            // "AreaPercentage": 100,
            // "ChargingTimePercentage": 100,
            "MagnitudePercentage": 130,
            "DurationPercentage": 130,
            "CostPercentage": 130
        },
        "Other":{
            // "AreaPercentage": 100,
            // "ChargingTimePercentage": 100,
            "MagnitudePercentage": 130,
            "DurationPercentage": 130,
            "CostPercentage": 150
        }
    },
    {
        "Name":"Ivory",
        "Description":"Channeling Magicka thought this staff: {Type} spells 50% less to cast, other spells cost 30% less to cast",
        "ForswornDescription":"Channeling Magicka thought this staff: Spells 70% less to cast",
        "FalmerDescription":"Channeling Magicka thought this staff: Spells are 20% stronger and cost 30% less to cast",
        "Main":{
            // "AreaPercentage": 100,
            // "MagnitudePercentage": 100,
            // "DurationPercentage": 100,
            // "ChargingTimePercentage": 100
            "CostPercentage": 50
        },
        "Other":{
            // "AreaPercentage": 100,
            // "MagnitudePercentage": 100,
            // "DurationPercentage": 100,
            // "ChargingTimePercentage": 100,
            "CostPercentage": 70
        }
    },
    {
        "Name":"Mystic",
        "Description":"Channeling Souls thought this staff: Non {Type} spells cost 20% more to cast",
        "ForswornDescription":"Channeling Souls thought this staff: Spells cost 20% lass to cast",
        "FalmerDescription":"Channeling Souls thought this staff: Spells are 20% stronger but cost 20% more to cast",
        "Main":{
            // "MagnitudePercentage": 100,
            // "AreaPercentage": 100,
            // "DurationPercentage": 100,
            // "CostPercentage": 100,
            // "ChargingTimePercentage": 100,
            "CostActorValue": "charge"
        },
        "Other":{
            // "MagnitudePercentage": 100,
            // "AreaPercentage": 100,
            // "DurationPercentage": 100,
            // "ChargingTimePercentage": 100,
            "CostPercentage": 120,
            "CostActorValue": "charge"
        }
    }
]
const result = []
let descriptions = ""
for(const value of values){
    const {Main, Other, Name,Description, ForswornDescription, FalmerDescription} = value
    for(const type of types){
        const obj = {
            "Keyword":`IS_${Name}${type}`
        }
        let desc;
        if(type == "Forsworn"){
            const current = {...Main}

            if(current["CostPercentage"] == null) current["CostPercentage"] = 100

            current["CostPercentage"] -= 20;

            obj["Default"] = current
            desc = ForswornDescription.replace(/\{Type\}\s/gi, type);
        } else if (type == "Falmer"){
            const current = {...Other}

            if(current["MagnitudePercentage"] == null) current["MagnitudePercentage"] = 100
            if(current["DurationPercentage"] == null) current["DurationPercentage"] = 100

            current["MagnitudePercentage"] += 20;
            current["DurationPercentage"] += 20;

            obj["Default"] = current
            desc = FalmerDescription.replace(/\{Type\}\s/gi, type);
        }
        else
        {
            obj[type] = Main
            obj["Default"] = Other
            desc = Description.replace(/\{Type\}/gi, type);
        }
        desc = desc.replace(/(\d+|Stamina|Magicka|Souls|Health|Alteration|Conjuration|Restoration|Destruction|Illusion)/gi, "<font color=\"#FFFFFF\">$1</font>");
        descriptions += `IS_${Name}Staff${type}|${desc}\n`;
        result.push(obj)
    }
}
fs.writeFileSync("ImmersiveStaves_ISTV.json", JSON.stringify(result, null, 4))
fs.writeFileSync("ImmersiveStaves_DESC.ini", descriptions)