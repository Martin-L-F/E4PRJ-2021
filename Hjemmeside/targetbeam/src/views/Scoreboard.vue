<template>
  <div class="scoreboard">
    <div class="rings"></div>
    <div class="rings"></div>
    <div class="lines"></div>
    <div class="lines"></div>
    <transition-group name="flip-list">
      <div v-for="user in usersSorted" :key="user.email">
        <h3>
          <span class="focus-color">Current score</span>
          <span class="slim"> | {{ user.name }}</span>
        </h3>
        <h1>{{ user.curentScore }}</h1>
      </div>
    </transition-group>
  </div>
</template>

<script>
export default {
  computed: {
    users: function() {
      return this.$store.getters.getUsers;
    },
    usersSorted() {
      var localUsers = this.users;

      return localUsers.sort((a, b) => {
        if (a.curentScore > b.curentScore) return -1;
        if (a.curentScore < b.curentScore) return 1;
        return 0;
      });
    }
  }
};
</script>

<style lang="scss" scoped>
.scoreboard {
  h3 {
    margin: 0 0 calc(var(--viewport-size) * -0.025) 0;
    font-size: calc(var(--viewport-size) * 0.025);
    z-index: 4;
    position: relative;
  }
  h1 {
    z-index: 4;
    position: relative;
    text-align: center;
    font-size: calc(var(--viewport-size) * 0.15);
    padding-left: 0.1em;
    margin: 0;
    text-shadow: 0 0 0.3em var(--white-3);
  }
}
</style>
