// variables
var sysInfoJson = "";
var cmdInfoJson = "";
var updateAutoIrrSec = true;
var updateDurationEn = true;
var waitingForResponse = false;
var unsuccessfulTries = 0;
var sysInfo;
var cmdInfo;
var client;

sysUrl = "http://sed-smarthome.ir/karkevand/httpProt/getInfoWeb.php?file=sys";
cmdUrl = "http://sed-smarthome.ir/karkevand/httpProt/getInfoWeb.php?file=cmd";
postCmdUrl = "http://sed-smarthome.ir/karkevand/httpProt/postInfo.php";

getCmdInfo(cmdUrl);
getInfo(sysUrl);

// get info every 3 sec
var t = setInterval(getInfo, 3000, sysUrl);

//asign functions to buttons
document.getElementById("valveButton").onclick = vlvBtnClick;
document.getElementById("autoIrrButton").onclick = autoIrrBtnClick;
document.getElementById("autoIrrSave").onclick = saveBtnClick;

//------------functions------------\\
//------------functions------------\\
//------------functions------------\\
//------------functions------------\\
//------------functions------------\\
//------------functions------------\\

function getInfo(url) {
  fetch(url)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok: " + response.statusText);
      }
      return response.text(); // or response.json() depending on the server's response
    })
    .catch((error) => {
      // Handle any errors
      console.error("Fetch error:", error);
    })

    .then((data) => {
      // Handle the response data
      sysInfoJson = data;
      sysInfo = JSON.parse(sysInfoJson);
      console.log("sysInfo:\n", sysInfoJson);
      if (!waitingForResponse) updateUI();
      if (sysInfo.copy && waitingForResponse) {
        waitingForResponse = false;
        updateUI();
      }
    });
}

function getCmdInfo(url) {
  fetch(url)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok: " + response.statusText);
      }
      return response.text(); // or response.json() depending on the server's response
    })
    .catch((error) => {
      // Handle any errors
      console.error("Fetch error:", error);
    })
    .then((data) => {
      // Handle the response data
      cmdInfoJson = data;
      console.log("cmdInfo:\n" + cmdInfoJson);
      cmdInfo = JSON.parse(cmdInfoJson);
    });
}

function updateUI() {
  //hide disConPic
  document.getElementById("disConDiv").classList.add("displayNone");
  document.getElementById("disConDiv").style.display = "none";

  sysInfo = JSON.parse(sysInfoJson);

  //update connection status
  updateConStat();

  //update humidity
  updateHumidity();

  // update duration
  if (updateDurationEn) updateDuration();

  //update valve status
  updateVlvStat();

  //update duration

  //update autoIrrEn
  if (updateAutoIrrSec) updateAutoIrrEn();
}

function updateConStat() {
  document.getElementById("conStatus").textContent = "متصل";
  document.getElementById("conStatus").style.marginRight = "35px";
  document
    .getElementsByClassName("ring-container")[0]
    .classList.remove("displayNone");
}

function updateHumidity() {
  document.getElementsByName("loadingPic")[0].classList.add("displayNone");
  if (sysInfo.humidity > 70) {
    document.getElementById("humQuality").textContent = "خوب";
    document.getElementById("checkedPic").classList.remove("displayNone");
    document.getElementById("warningPic").classList.add("displayNone");
    document.getElementById("errorPic").classList.add("displayNone");
  }
  if (sysInfo.humidity < 70 && sysInfo.humidity > 30) {
    document.getElementById("humQuality").textContent = "متوسط";
    document.getElementById("checkedPic").classList.add("displayNone");
    document.getElementById("warningPic").classList.remove("displayNone");
    document.getElementById("errorPic").classList.add("displayNone");
  }
  if (sysInfo.humidity < 30) {
    document.getElementById("humQuality").textContent = "کم";
    document.getElementById("checkedPic").classList.add("displayNone");
    document.getElementById("warningPic").classList.add("displayNone");
    document.getElementById("errorPic").classList.remove("displayNone");
  }

  document.getElementsByName("loadingPic")[1].classList.add("displayNone");
  document.getElementById("humidityPercent").textContent =
    sysInfo.humidity + "%";
}
function updateDuration() {
  updateDurationEn = false;
  var m = 0;
  var h = 0;
  h = Math.floor(sysInfo.duration / 60);
  if (h.toString().length < 2) h = "0" + h;
  m = sysInfo.duration % 60;
  if (m.toString().length < 2) m = "0" + m;
  document.getElementById("durationMin").value = m;
  document.getElementById("durationHour").value = h;
}

function updateVlvStat() {
  if (sysInfo.valve) valveIsOpen();
  else valveIsClose();
}

function valveIsOpen() {
  // update H3
  document.getElementsByName("loadingPic")[2].classList.add("displayNone");
  document.getElementById("valveStatus").textContent = "شیر باز است";
  document.getElementById("valveStatus").classList.add("greenH3");
  document.getElementById("valveStatus").classList.remove("redH3");

  // update button
  document.getElementById("valveButton").textContent = "بستن";
  document.getElementById("valveButton").classList.remove("loadingButton");
  document.getElementById("valveButton").classList.add("redButton");
  document.getElementById("valveButton").classList.remove("greenButton");
  document.getElementById("valveButton").removeAttribute("disabled");

  // update duration section
  document.getElementById("irrTimeTd").classList.add("displayNone");
  document.getElementById("irrigating").classList.remove("displayNone");
  document.getElementById("durationTimeDiv").classList.add("displayNone");
  document.getElementsByName("loadingPic")[2].classList.add("displayNone");
}

function valveIsClose() {
  // update H3
  document.getElementsByName("loadingPic")[2].classList.add("displayNone");
  document.getElementById("valveStatus").textContent = "شیر بسته است";
  document.getElementById("valveStatus").classList.remove("greenH3");
  document.getElementById("valveStatus").classList.add("redH3");

  // update button
  document.getElementById("valveButton").textContent = "باز کردن";
  document.getElementById("valveButton").classList.remove("loadingButton");
  document.getElementById("valveButton").classList.remove("redButton");
  document.getElementById("valveButton").classList.add("greenButton");
  document.getElementById("valveButton").removeAttribute("disabled");

  //update duration section
  document.getElementById("irrTimeTd").classList.remove("displayNone");
  document.getElementById("irrigating").classList.add("displayNone");
  document.getElementById("durationTimeDiv").classList.remove("displayNone");
  document.getElementsByName("loadingPic")[3].classList.add("displayNone");
}

function updateAutoIrrEn() {
  updateAutoIrrSec = false;
  if (sysInfo.autoIrrEn) {
    // update H3
    document.getElementById("autoIrrEn").textContent = "آبیاری خودکار فعال است";
    document.getElementById("autoIrrEn").classList.remove("redH3");
    document.getElementById("autoIrrEn").classList.add("greenH3");
    document.getElementsByName("loadingPic")[4].classList.add("displayNone");

    // update button
    document.getElementById("autoIrrButton").removeAttribute("disabled");
    document.getElementById("autoIrrButton").textContent = "خاموش کردن";
    document.getElementById("autoIrrButton").classList.remove("loadingButton");
    document.getElementById("autoIrrButton").classList.remove("greenButton");
    document.getElementById("autoIrrButton").classList.add("redButton");

    // calculate next irr
    var nextIrrTS = sysInfo.lastIrrTS + sysInfo.howOften * 24 * 60 * 60;
    var options = { year: "numeric", month: "numeric", day: "numeric" };
    var date = new Date(nextIrrTS * 1000).toLocaleDateString("fa-IR", options);

    document.getElementsByName("loadingPic")[5].classList.add("displayNone");
    document.getElementsByName("crossPic")[0].classList.add("displayNone");
    document.getElementById("nextIrr").classList.remove("displayNone");
    document.getElementById("nextIrr").textContent = date;

    // update irrigation howOften
    document.getElementsByName("loadingPic")[6].classList.add("displayNone");
    document.getElementsByName("crossPic")[1].classList.add("displayNone");
    document.getElementById("howOftenDiv").classList.remove("displayNone");
    document.getElementById("howOften").value = sysInfo.howOften;

    // update irrigation clock
    document.getElementsByName("loadingPic")[7].classList.add("displayNone");
    document.getElementsByName("crossPic")[2].classList.add("displayNone");
    document.getElementById("irrClockDiv").classList.remove("displayNone");
    h = sysInfo.hour;
    if (h.toString().length < 2) h = "0" + h;
    m = sysInfo.minute;
    if (m.toString().length < 2) m = "0" + m;
    document.getElementById("minute").value = m;
    document.getElementById("hour").value = h;

    //update irrigation duration
    document.getElementsByName("loadingPic")[8].classList.add("displayNone");
    document.getElementsByName("crossPic")[3].classList.add("displayNone");
    document.getElementById("irrTimeDiv").classList.remove("displayNone");
    var m = 0;
    var h = 0;
    h = Math.floor(sysInfo.duration / 60);
    if (h.toString().length < 2) h = "0" + h;
    m = sysInfo.duration % 60;
    if (m.toString().length < 2) m = "0" + m;
    document.getElementById("AIdurationMin").value = m;
    document.getElementById("AIdurationHour").value = h;

    // update save button
    document.getElementById("autoIrrSave").classList.remove("loadingButton");
    document.getElementById("autoIrrSave").classList.add("saveButton");
    document.getElementById("autoIrrSave").textContent = "ذخیره";
    document.getElementById("autoIrrSave").removeAttribute = "disabled";
  } else {
    // update H3
    document.getElementById("autoIrrEn").textContent =
      "آبیاری خودکار خاموش است";
    document.getElementById("autoIrrEn").classList.add("redH3");
    document.getElementById("autoIrrEn").classList.remove("greenH3");
    document.getElementsByName("loadingPic")[4].classList.add("displayNone");

    // update button
    document.getElementById("autoIrrButton").removeAttribute("disabled");
    document.getElementById("autoIrrButton").textContent = "روشن کردن";
    document.getElementById("autoIrrButton").classList.remove("loadingButton");
    document.getElementById("autoIrrButton").classList.add("greenButton");
    document.getElementById("autoIrrButton").classList.remove("redButton");

    // update next irr section
    document.getElementsByName("loadingPic")[5].classList.add("displayNone");
    document.getElementsByName("crossPic")[0].classList.remove("displayNone");
    document.getElementById("nextIrr").classList.add("displayNone");

    // update irrigation howOften
    document.getElementsByName("loadingPic")[6].classList.add("displayNone");
    document.getElementsByName("crossPic")[1].classList.remove("displayNone");
    document.getElementById("howOftenDiv").classList.add("displayNone");

    // update irrigation clock
    document.getElementsByName("loadingPic")[7].classList.add("displayNone");
    document.getElementsByName("crossPic")[2].classList.remove("displayNone");
    document.getElementById("irrClockDiv").classList.add("displayNone");

    //update irrigation duration
    document.getElementsByName("loadingPic")[8].classList.add("displayNone");
    document.getElementsByName("crossPic")[3].classList.remove("displayNone");
    document.getElementById("irrTimeDiv").classList.add("displayNone");

    // update save button
    document.getElementById("autoIrrSave").classList.add("loadingButton");
    document.getElementById("autoIrrSave").classList.remove("saveButton");
    document.getElementById("autoIrrSave").textContent = "ذخیره";
    document.getElementById("autoIrrSave").setAttribute = "disabled";
  }
}

function vlvBtnClick() {
  // release duration update En
  updateDurationEn = true;
  //wait for valve open response
  waitingForResponse = true;

  // check if valve is open
  if (sysInfo.valve) {
    console.log("Closing Valve");

    //prepare data to post
    cmdInfo.valveCmd = "close";
    cmdInfoJson = JSON.stringify(cmdInfo);

    //post new data to cmdInfo
    post();
  } else {
    //prepare data to send

    var hour = document.getElementById("durationHour").value;
    var min = document.getElementById("durationMin").value;
    var irrDuration = Number(hour) * 60 + Number(min);
    cmdInfo.durationCmd = irrDuration;
    cmdInfo.valveCmd = "open";
    cmdInfoJson = JSON.stringify(cmdInfo);

    //post new data to cdmInfo
    post();
  }

  // update button css
  document.getElementById("valveButton").classList.remove("greenButton");
  document.getElementById("valveButton").classList.remove("redButton");
  document.getElementById("valveButton").classList.add("loadingButton");
  document.getElementById("valveButton").textContent = "در حال ارسال...";
}

function autoIrrBtnClick() {
  // release auto irr section to be updated
  updateAutoIrrSec = true;
  //wait for valve open response
  waitingForResponse = true;
  //check if auto irr was enable or not
  if (sysInfo.autoIrrEn) {
    //prepare data to post in cmdInfo
    cmdInfo.autoIrrEnCmd = false;
    cmdInfoJson = JSON.stringify(cmdInfo);

    //post new data
    post();
  } else {
    // prepare data to send to cmdInfo
    var AIhour = sysInfo.hour;
    var AImin = sysInfo.minute;
    var irrHowOften = sysInfo.howOften;
    var irrDuration = sysInfo.duration;

    cmdInfo.autoIrrEnCmd = 1;
    cmdInfo.durationCmd = irrDuration;
    cmdInfo.minuteCmd = AImin;
    cmdInfo.hourCmd = AIhour;
    cmdInfo.howOftenCmd = irrHowOften;
    cmdInfoJson = JSON.stringify(cmdInfo);

    // post new data
    post();
  }

  //update button
  document.getElementById("autoIrrButton").classList.remove("greenButton");
  document.getElementById("autoIrrButton").classList.remove("redButton");
  document.getElementById("autoIrrButton").classList.add("loadingButton");
  document.getElementById("autoIrrButton").textContent = "در حال ارسال...";

  // waitForResponse();
}

function saveBtnClick() {
  // release auto irr section to be updated
  updateAutoIrrSec = true;
  //wait for valve open response
  waitingForResponse = true;

  // prepare data to post in cmdInfo
  var hour = document.getElementById("AIdurationHour").value;
  var min = document.getElementById("AIdurationMin").value;
  var irrHowOften = document.getElementById("howOften").value;
  var AImin = document.getElementById("minute").value;
  var AIhour = document.getElementById("hour").value;
  var irrDuration = Number(hour) * 60 + Number(min);
  cmdInfo.autoIrrEnCmd = sysInfo.autoIrrEn;
  cmdInfo.durationCmd = irrDuration;
  cmdInfo.minuteCmd = AImin;
  cmdInfo.hourCmd = AIhour;
  cmdInfo.howOftenCmd = irrHowOften;

  cmdInfoJson = JSON.stringify(cmdInfo);
  // post new data to cmdInfo
  post();

  //update button
  document.getElementById("autoIrrSave").classList.remove("greenButton");
  document.getElementById("autoIrrSave").classList.remove("redButton");
  document.getElementById("autoIrrSave").classList.add("loadingButton");
  document.getElementById("autoIrrSave").textContent = "در حال ارسال...";
}

function insertIntoDB() {
  var bracketIndex = sysInfoJson.indexOf("{");
  if (bracketIndex != -1) {
    sysInfoJson = sysInfoJson.substring(bracketIndex);
    sysInfo = JSON.parse(sysInfoJson);
  }
  var sendData = { duration: sysInfo.duration };
  var sendDataJson = JSON.stringify(sendData);
  console.log(sendDataJson);
  fetch("http://sed-smarthome.ir/karkevand/php/insertToDb.php", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: "insertIntoDB=" + sendDataJson,
  }).then((res) => {
    console.log("Request complete! response:", res);
  });
}

function post() {
  fetch(postCmdUrl, {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: "cmdInfo=" + cmdInfoJson,
  }).then((res) => {
    console.log("Request complete! response:", res);
  });
}
