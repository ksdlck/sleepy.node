sleepy = new (require("../lib/"));

console.log("Sleeping for a while...");
sleepy.sleep(1, function(){
  console.log("Done sleeping!");
});
sleepy[0];
