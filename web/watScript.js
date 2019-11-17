let bubbling = 0;
let color = "#FF0000";
let minBubbling = .25;
let maxBubbling = .75;

function refreshData()
{
    console.log("Refreshing data");
    // let text = readTextFile("data.txt");
    // readResponseText(text);
    let waterPctText = readTextFile("water_percent.txt");
    bubbling = 1 - readWaterPercent(waterPctText), 4;
    console.log(bubbling);
    let waterColorText = readTextFile("water_color_avg.txt");
    readWaterColorAvg(waterColorText);
    console.log(color);
    updateBubblingStatus();
    // document.getElementById("colorBox").innerHTML = color;
    updateColorStatus();
}

function round(value, decimals) 
{
    return Number(Math.round(value+'e'+decimals)+'e-'+decimals);
}

function updateBubblingStatus()
{
    minBubbling = document.getElementById("minBub").value / 100;
    maxBubbling = document.getElementById("maxBub").value / 100;

    document.getElementById("bubVal").innerHTML = round((100 * bubbling), 2) + "%";
    if(bubbling < minBubbling || bubbling > maxBubbling)
    {
        document.getElementById("bubStatus").innerHTML = "Warning";
        document.getElementById("bubStatus").style.backgroundColor = "yellow";
    }
    else
    {
        document.getElementById("bubStatus").innerHTML = "Good";
        document.getElementById("bubStatus").style.backgroundColor = "lawngreen";
    }
    
}

function updateColorStatus()
{
    document.getElementById("colorBox").style.backgroundColor = color;
    document.getElementById("colorStatus").style.backgroundColor = "lawngreen"
}

function readTextFile(file)
{
    let rawFile = new XMLHttpRequest();
    let text;
    rawFile.open("GET", file, false);
    rawFile.onreadystatechange = function ()
    {
        if(rawFile.readyState === 4)
        {
            if(rawFile.status === 200 || rawFile.status == 0)
            {
                text = rawFile.responseText; 
            }
        }
    }
    rawFile.send(null);
    return text;
}

function readResponseText(text)
{
    let txtList = text.split(/\r?\n/);
    console.log("txtList: " + txtList);
    bubbling = parseFloat(txtList[0]);
    console.log("bubbling: " + bubbling);
    let redHexStr = hexFormat(parseInt(txtList[1]));
    let greenHexStr = hexFormat(parseInt(txtList[2]));
    let blueHexStr = hexFormat(parseInt(txtList[3]));
    color = "#" + redHexStr + greenHexStr + blueHexStr;
    console.log("color: " + color);
}

function readWaterPercent(text)
{
    let pctList = text.split(/\r?\n/);
    let sum = 0.0;
    for(let i = 0; i < pctList.length; i++)
    {
        let num = parseFloat(pctList[i]);
        if(num)
        {
            sum += num;
        }
    }
    return sum / pctList.length;
}

function readWaterColorAvg(text)
{
    let colorsList = text.split(/\r?\n/);
    let sumsList = [0, 0, 0];
    for(let i = 0; i < colorsList.length; i++)
    {
        let color = colorsList[i].split(",");
        for(let j = 0; j < 3; j++)
        {
            if(color[j])
            {
                sumsList[j] += color[j] / colorsList.length;
            }
        }
    }
    console.log(sumsList);
    let blueHexStr = hexFormat(Math.round(parseFloat(sumsList[0])));
    let greenHexStr = hexFormat(Math.round(parseFloat(sumsList[1])));
    let redHexStr = hexFormat(Math.round(parseFloat(sumsList[2])));
    console.log(Math.round(parseFloat(sumsList[0])))
    console.log(redHexStr);
    color = "#" + redHexStr + greenHexStr + blueHexStr;
    console.log(color);
}

function hexFormat(num)
{
    s = num.toString(16);
    if(s.length < 2)
    {
        s = '0' + s;
    }
    return s;
}