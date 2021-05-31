<template>
  <div>
    <div class="rings"></div>
    <div class="rings"></div>
    <div class="lines"></div>
    <div class="lines"></div>
    <h1>{{ count }}</h1>
  </div>
</template>

<script>
import { send } from "@/ws_client.js";
export default {
  data() {
    return {
      count: 3,
      intervalHandle: 0
    };
  },
  mounted() {
    this.intervalHandle = setInterval(() => {
      this.count--;
      if (this.count <= 0) {
        clearInterval(this.intervalHandle);
        send({
          type: "CountdownOver"
        }, () =>{
            this.$router.push("/scoreboard");
        });
      }
    }, 1000);
  }
};
</script>

<style lang="scss" scoped>
h1 {
  font-size: calc(var(--viewport-size) * 0.5);
  letter-spacing: 0;
  margin: 0;
  position: relative;
}
</style>
