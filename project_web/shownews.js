var news = new Array();

news[36] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Mar. 2017 </td><td>&nbsp; WaveCube accepted to IEEE/ACM Transactions on Networking</td></tr>';

news[35] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Feb. 2017 </td><td>&nbsp; PIAS accepted to IEEE/ACM Transactions on Networking</td></tr>';

news[34] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Jan. 2017 </td><td>&nbsp; Submit your work to <a href="http://conferences.sigcomm.org/events/apnet2017/index.html">Asia-Pacific Workshop on Networking (<font color="red">APNet\'17</font>)</a></td></tr>';

news[33] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Dec. 2016 </td><td>&nbsp; MegaSwitch accepted to NSDI\'17 (Congrats SING!)</td></tr>';

news[32] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Oct. 2016 </td><td>&nbsp; Li wins Microsoft Research Asia PhD Fellowship Award (Congratulations Li!)</td></tr>';

news[31] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Sept. 2016 </td><td>&nbsp; TCN accepted to CoNEXT\'16</td></tr>';

news[30] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Sept. 2016 </td><td>&nbsp; RDMA Deadlock accepted to HotNets\'16</td></tr>';

news[29] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Sept. 2016 </td><td>&nbsp; Hong wins Google PhD Fellowship Award (Congratulations Hong!)</td></tr>';

news[28] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; July 2016 </td><td>&nbsp; Amoeba accepted to IEEE/ACM Transactions on Networking</td></tr>';

news[27] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; July 2016 </td><td>&nbsp; Stream accepted to ICNP\'16</td></tr>';

news[26] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; April 2016 </td><td>&nbsp; Karuna accepted to SIGCOMM 2016 (Congratulations SING!)</td></tr>';

news[25] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; April 2016 </td><td>&nbsp; CODA accepted to SIGCOMM 2016 (Congratulations SING!)</td></tr>';

news[24] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Feb. 2016 </td><td>&nbsp; As part of MQ-ECN, we release a simple <a href="https://github.com/HKUST-SING/TrafficGenerator">traffic generator</a> for network experiments.</td></tr>';

news[23] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Dec. 2015 </td><td>&nbsp; MQ-ECN accepted to NSDI 2016 (Congratulations SING!)</td></tr>';

news[22] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Nov. 2015 </td><td>&nbsp; Trinity accepted to INFOCOM 2016</td></tr>';

news[21] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Nov. 2015 </td><td>&nbsp; Optas accepted to INFOCOM 2016</td></tr>';

news[20] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Sept. 2015 </td><td>&nbsp; Wei wins MSRA PhD Fellowship Award</td></tr>';

news[19] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Sept. 2015 </td><td>&nbsp; XPath accepted to IEEE/ACM Transactions on Networking</td></tr>';

news[18] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Jul. 2015 </td><td>&nbsp; HadoopWatch accepted to IEEE/ACM Transactions on Networking</td></tr>';

news[17] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Mar. 2015 </td><td>&nbsp; FlowProphet accepted to ICDCS 2015</td></tr>';

news[16] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Jan. 2015 </td><td>&nbsp; Amoeba accepted to EuroSys 2015</td></tr>';

news[15] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Dec. 2014 </td><td>&nbsp; PIAS accepted to NSDI 2015, Congratulations to our SING team!</td></tr>';

news[14] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Dec. 2014 </td><td>&nbsp; XPath accepted to NSDI 2015, Congratulations to our SING team!</td></tr>';

news[13] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Nov. 2014 </td><td>&nbsp; RAPIER accepted to INFOCOM 2015</td></tr>';

news[12] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Nov. 2014 </td><td>&nbsp; WaveCube accepted to INFOCOM 2015</td></tr>';

news[11] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Sept. 2014 </td><td>&nbsp; PIAS in ACM HotNets 2014</td></tr>';

news[10] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Jul. 2014 </td><td>&nbsp; PAC in IEEE ICNP 2014</td></tr>';

news[9] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Apr. 2014 </td><td>&nbsp; BitBill in USENIX HotCloud 2014</td></tr>';

news[8] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Mar. 2014 </td><td>&nbsp; PC member for SIGCOMM\'14 poster/demo, HotCloud\'14, INFOCOM\'15</td></tr>';

news[7] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Nov. 2013 </td><td>&nbsp; HadoopWatch in IEEE INFOCOM 2014</td></tr>';

news[6] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Sept. 2013 </td><td>&nbsp; MCP accepted to ACM HotNets 2013</td></tr>';

news[5] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Jul. 2013 </td><td>&nbsp; SideWalk accepted to IEEE Transactions on Computers</td></tr>';

news[4] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; May 2013 </td><td>&nbsp; DISCO accepted to IEEE/ACM Transactions on Networking</td></tr>';

news[3] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; May 2013 </td><td>&nbsp; PC member for SIGCOMM\'13 poster/demo sessions</td></tr>';

news[2] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Mar. 2013 </td><td>&nbsp; OSA accepted to IEEE/ACM Transactions on Networking</td></tr>'; 

news[1] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Mar. 2013 </td><td>&nbsp; PC member for INFOCOM\'14</td></tr>'; 

news[0] = '<tr><td align=right>&nbsp;&nbsp;&nbsp; Jul. 2012 </td><td>&nbsp; Join the CSE department of HKUST</td></tr>'; 


var maxnewscnt = 12;
function showNews(partial) {
  len = news.length;
  if (partial == true) {
    len = Math.min (news.length, maxnewscnt);
    xtra='<tr><td></td><td>&nbsp; <a onclick="javascript:showNews(false)">More...</a></td></tr>';
  }
  else {
    len = news.length;
    xtra='<tr><td></td><td>&nbsp; <a onclick="javascript:showNews(true)">Less...</a></td></tr>';
  }

  elt = "<table border=0>";
  for (var i=0; i < len; i++)
    elt = elt + news[news.length-i-1] + '\n';
  elt = elt + xtra + "\n </table>";
  document.getElementById("news").innerHTML=elt;
}

function init() {
  var partialnews = true;

  var url = window.location.toString();
  url.match(/\?(.+)$/);
  var params = RegExp.$1.split("&");

  for (var i=0; i <params.length; i++)
    if (params[i] == "news")
      partialnews = false;

  showNews(partialnews);
}
window.onload = init; 
