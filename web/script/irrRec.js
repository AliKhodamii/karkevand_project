tableData = [];

httpReqAndCreateRecIrrTable();

function httpReqAndCreateRecIrrTable() {
  const Http = new XMLHttpRequest();
  const url = "http://sed-smarthome.ir/karkevand/php/getIrrRec.php";
  Http.open("GET", url);
  Http.send();

  Http.onreadystatechange = (e) => {
    // console.log(Http.responseText);
    tableData = JSON.parse(Http.responseText);
    // console.log(tableData);
    createTable();
  };
}
function createTable() {
  var html = "";
  for (var i = 0; i < tableData.length; i++) {
    html += "<tr>";

    html += "<td>" + (i + 1).toString() + "</td>";
    html += "<td>" + tableData[i].farsiDay + "</td>";
    html += "<td>" + tableData[i].date + "</td>";
    html += "<td>" + tableData[i].time + "</td>";
    html += "<td>" + tableData[i].duration + "</td>";

    html += "</tr>";
  }
  document.getElementById("recIrr").innerHTML = html;
}
