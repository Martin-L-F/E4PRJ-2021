// TTS - variables
var voices = null;
var selectedVoice = null;
var utterance = null;

// Initiating bots TTS and STT functionality
export function initiateSpeech() {
	// ===== TTS ===== //
	// Find voices
	voices = speechSynthesis.getVoices();
	// Search for the good voice
	var voice = voices.filter(voice => {
		return voice.name == 'Google US English';
	});
	if (voice.length > 0) {
		selectedVoice = voice[0];
		console.log('[SPEAK] Voice initiation complete');
	} else {
		// If it isn't there, take the default and try again in 100 ms
		selectedVoice = voices[0];
		setTimeout(initiateSpeech, 100);
	}
	utterance = new SpeechSynthesisUtterance();
	utterance.voice = selectedVoice; // Apply voice
}

// Function that speaks the input
export function speak(input) {
	utterance.text = input;
	console.log('[SPEAK] ' + input);
	speechSynthesis.speak(utterance);
}
