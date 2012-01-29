Sleepy = require("../lib/");

(function() {
  sleepy = new Sleepy(1);
  console.log("Sleeping for a while...");
  sleepy.sleep(function(){
    console.log("Done sleeping!");
    sleepy[0];
  });
})();

(function() {
  sleepy = new Sleepy(2);
  console.log("Waiting for the other guy...");
  sleepy.sleep(function(){
    console.log("That looks like the end.");
  });
})();
