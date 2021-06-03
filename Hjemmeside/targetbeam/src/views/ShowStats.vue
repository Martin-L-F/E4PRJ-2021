<template>
  <div>
    <h1>The winner is {{ usersSorted[0].name }}</h1>
    <div class="flex-content">
      <div
        class="box userScoreBoard"
        v-for="user in usersSorted"
        :key="user.email"
        :style="'--i:' + user.curentScore"
      >
        <h3>{{ user.name }}</h3>
        <h1>{{ user.curentScore }}</h1>
      </div>
    </div>
    <div class="btn" v-on:click="$router.push('/')">GO HOME</div>
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
.flex-content {
  display: flex;
  justify-content: center;
  gap: 2rem;
  margin-bottom: 4rem;
  .userScoreBoard {
    height: 400px;
    width: 250px;
    text-align: center;
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    h3 {
    }
    h1 {
      font-size: 3rem;
    }

    position: relative;
    & > * {
      position: relative;
      z-index: 2;
    }
    &::before {
      transition: top 300ms ease, background-color 300ms ease,
        box-shadow 300ms ease, border-color 300ms ease;
      z-index: 0;
      content: "";
      position: absolute;
      display: block;
      right: -2px;
      left: -2px;
      bottom: -2px;
      top: calc((100 - var(--i)) * 1% - 2px);
      background-color: var(--primary-2);

      border: 2px solid var(--primary-2);
      box-shadow: 0 0 16px 0px var(--primary-5),
        inset 0 0 16px 0px var(--primary-3);
    }
  }
}
.btn {
  max-width: max-content;
  margin-left: auto;
  margin-right: auto;
}
</style>
