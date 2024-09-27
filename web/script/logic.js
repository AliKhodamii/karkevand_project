var subTopic = "karSSG/ESP";
var pubTopic = "karSSG/client";
var sysInfoJson = "";
var updateAutoIrrSec = true;
var updateDurationEn = true;

var sysInfo;

var client;

document.getElementById("valveButton").onclick = vlvBtnClick;

// connect to mqtt broker
mqttConnect();

function mqttConnect() {
  client = mqtt.connect("ws://test.mosquitto.org:8080");

  client.on("connect", () => {
    console.log("Connected to broker");
    client.subscribe(subTopic, (err) => {
      if (!err) {
        client.publish(pubTopic, "Hello from local MQTT.js");
      }
    });
  });
  client.on("message", (topic, message) => {
    sysInfoJson = message.toString();
    console.log("Received message:", sysInfoJson);

    // update ui if info came
    // if (sysInfoJson.indexOf("info") != -1) {
    updateUI();
    // }
  });
}
function updateUI() {
  //decode json
  // sysInfoJson = sysInfoJson.substring(sysInfoJson.indexOf("info") + 4);
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
  doUpdateDuration = false;
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
  if (sysInfo.autoIrrigationEn) {
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
    var nextIrrTS = sysInfo.lastIrrigationTS + sysInfo.howOften * 24 * 60 * 60;
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
  // check if valve is open

  if (sysInfo.valve) {
    console.log("Closing the valve");
    client.subscribe(subTopic, (err) => {
      if (!err) {
        client.publish(pubTopic, "close valve");
      }
    });

    // update button css
    document.getElementById("valveButton").classList.remove("redButton");
    document.getElementById("valveButton").classList.add("loadingButton");
    document.getElementById("valveButton").textContent = "در حال ارسال...";

    // release duration update En
    updateDurationEn = true;

    // be in loop until the response comes
    while (1) {
      client.on("message", (topic, message) => {
        sysInfoJson = message.toString();
        console.log("Received message:", sysInfoJson);
      });
      // listen for response
      if (sysInfoJson.indexOf("valve is close") != -1) {
        // updateUI();
        break;
      }
    }
  } else {
    // create a json to send
    var cmd = {};
    var hour = document.getElementById("durationHour").value;
    var min = document.getElementById("durationMin").value;
    var irrDuration = Number(hour) * 60 + Number(min);
    cmd.valve = 1;
    cmd.duration = irrDuration;
    vlvOpenCmd = "valve open" + JSON.stringify(cmd);

    console.log(vlvOpenCmd);

    // publish open cmd
    client.subscribe(subTopic, (err) => {
      if (!err) {
        client.publish(pubTopic, vlvOpenCmd);
      }
    });

    // update button css
    document.getElementById("valveButton").classList.remove("greenButton");
    document.getElementById("valveButton").classList.add("loadingButton");
    document.getElementById("valveButton").textContent = "در حال ارسال...";

    // release duration update En
    updateDurationEn = true;

    // be in loop until the response comes
    while (1) {
      client.on("message", (topic, message) => {
        sysInfoJson = message.toString();
        console.log("Received message:", sysInfoJson);
      });
      // listen for response
      if (sysInfoJson.indexOf("valve is open") != -1) {
        // updateUI();
        break;
      }
    }
  }
}

function autoIrrBtnClick() {
  if (sysInfo.autoIrrigationEn) {
  } else {
    // create command json
    var cmd = {};
    var hour = document.getElementById("AIdurationHour").value;
    var min = document.getElementById("AIdurationMin").value;
    var irrHowOften = document.getElementById("howOften").value;
    var AImin = document.getElementById("min").value;
    var AIhour = document.getElementById("hour").value;
    var irrDuration = Number(hour) * 60 + Numner(min);
    cmd.autoIrrigationEn = 1;
    cmd.duration = irrDuration;
    cmd.min = AImin;
    cmd.hour = AIhour;
    cmd.howOften = irrHowOften;

    var updInfoJson = "enable autoIrrigation" + JSON.stringify(cmd);

    // publish open cmd
    client.subscribe(subTopic, (err) => {
      if (!err) {
        client.publish(pubTopic, updInfoJson);
      }
    });
  }

  //update button
  document.getElementById("autoIrrButton").classList.remove("greenButton");
  document.getElementById("autoIrrButton").classList.add("loadingButton");
  document.getElementById("autoIrrButton").textContent = "در حال ارسال...";

  //wait untill response comes
  while (true) {
    client.on("message", (topic, message) => {
      sysInfoJson = message.toString();
      console.log("Received message:", sysInfoJson);
    });
    // listen for response
    if (sysInfoJson.indexOf("autoIrrigation is enable") != -1) {
      // updateUI();
      break;
    }
  }
}

function saveBtnClick() {}
