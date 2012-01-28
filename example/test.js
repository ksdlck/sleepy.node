Sleepy = require("../lib/");

sleepy = new Sleepy(1);

console.log("Sleeping for a while...");
sleepy.sleep(function(){
  console.log("Done sleeping!");
  sleepy[0];
});
