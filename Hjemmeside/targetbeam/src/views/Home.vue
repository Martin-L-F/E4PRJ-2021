<template>
	<div>
		<div class="title">
			<h1>TARGET<span class="focus-color">BEAM</span></h1>
		</div>
		<div class="split">
			<div class="box stats">
				<p class="description">User:</p>
				<h1>Doe, John</h1>
				<p class="description">Ranking:</p>
				<h2>Sargent</h2>

				<div class="profile-img">
					<Gravatar :size="225" :email="'mart730d@hotmail.com'" />
				</div>
				<div class="split">
					<div class="btn sm" v-on:click="signout()">SIGN OUT</div>
				</div>
			</div>
			<div class="box flex-down">
				<h1 class="grow">Select a gamemode</h1>

				<div class="split">
					<div class="btn ritch">
						<h3>View statistics</h3>
						<p>Check out how you're doing.</p>
					</div>
					<div
						class="btn ritch"
						v-on:click="$router.push('/scoreboard')"
					>
						<h3>Campaign</h3>
						<p>
							Improve your skills in<br />
							an epic adventure.
						</p>
					</div>
					<div class="btn ritch">
						<h3>Co-up</h3>
						<p>Train with a friend.</p>
					</div>
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	import Gravatar from "@/components/gravatar.vue";
	export default {
		components: {
			Gravatar,
		},
		methods: {
			signout: function() {
				localStorage.removeItem("token");
				this.$router.push("/signin");
			},
		},
	};
</script>
